<?php

function rm_cart_checkout($form, &$form_state, $account) {
    global $user;
    $form = array();
    $form['checkout'] = array();
    
    /*
     *  Login erzwingen
     */
    rm_user_enforce_login(t('Please log in with your commercial account before you continue'), 'warning');
    
    /*
     *  Verkäufer UID in Session schreiben
     */
    $_SESSION['chosen_seller'] = $account->uid;
    
    /*
     *  One Click Einstellung in value-Feld speichern um diese nach dem Submit aktualisieren zu können
     *  steht auf -1 wenn keine Einstellung vorhanden ist und neu angelegt werden muss
     */
    $one_click_setting = rm_cart_get_one_click_setting($user->uid, $account->uid);
    $form['checkout']['oneclick'] = array(
        '#type' => 'value',
        '#value' => (!empty($one_click_setting)) ? $one_click_setting->nid : -1,
    );
    
    /*
     *  Verfügbare Lieferarten anhand der Agreements und Mindestbestellwerte prüfen
     */
    $minimum_order_values = array();
    $deliverytypes = array();
    $zipcode = NULL;
    if(isset($_SESSION['regionselect']['zip']) && !empty($_SESSION['regionselect']['zip'])) $zipcode = $_SESSION['regionselect']['zip'];
    $minimum_order_values = rm_cart_get_minimum_order_values($account->uid, $zipcode);
    $carttotal = rm_cart_get_cart_total($account->uid);
    if(isset($minimum_order_values)) {
        foreach($minimum_order_values as $type => $value) {
            foreach($value as $nid => $mov) {
                if($carttotal >= $mov) $deliverytypes[$type] = node_type_get_name($type);
            }
        }
    }
    
    /*
     *  Längsten Aufschub von allen Produkten ermitteln und als Maßstab für alle anderen Zeiten definieren
     *  $longestdelay
     */
    $cart_items = rm_cart_get_cart($account->uid, $user->uid);
    $longestdelay = 0;
    if(!empty($cart_items)) {
        foreach($cart_items as $cart_item) {
            $offer_variation = node_load($cart_item->field_offer_variation_reference[LANGUAGE_NONE][0]['target_id']);
            if(isset($offer_variation->field_leadtime[LANGUAGE_NONE][0]['value']) && !empty($offer_variation->field_leadtime[LANGUAGE_NONE][0]['value']) && $offer_variation->field_leadtime[LANGUAGE_NONE][0]['value'] > $longestdelay) {
                $longestdelay = $offer_variation->field_leadtime[LANGUAGE_NONE][0]['value'];
            }
        }
    }

    /*
     *  Feld für Lieferart
     */
    $form['checkout']['delivery_fs']['delivery'] = array(
        '#type' => 'radios',
        '#title' => t('Delivery option'),
        '#title_display' => 'none',
        '#options' => $deliverytypes,
    );
    if(count($deliverytypes) == 1) {
        $deliverytypeskeys = array_keys($deliverytypes);
        $form['checkout']['delivery_fs']['delivery']['#default_value'] = $deliverytypeskeys[0];
    }
    
    
    /*
     *  Abholvereinbarungen auslesen
     */
    $pickupspots = array();
    $pickuptimes = array();
    if(isset($minimum_order_values['pickup_agreement']) && !empty($minimum_order_values['pickup_agreement'])) {
        foreach($minimum_order_values['pickup_agreement'] as $agreement_id => $mov) {
            $agreement = node_load($agreement_id);
            
            //Pickup spots festlegen
            $pickupspots[$agreement->nid] = $agreement->field_address[LANGUAGE_NONE][0]['locality'] . ', ' . $agreement->field_address[LANGUAGE_NONE][0]['thoroughfare'];
            $surcharge = 0;
            if(!empty($agreement->field_surcharge[LANGUAGE_NONE][0]['value']) && $agreement->field_surcharge[LANGUAGE_NONE][0]['value'] > 0) $surcharge = $agreement->field_surcharge[LANGUAGE_NONE][0]['value'];
            $pickupspots[$agreement->nid] .= ($surcharge > 0) ? ' (+' . number_format($surcharge, 2, ",", ".") . '€)' : ' (kostenlos)';
            
            $regular_times = rm_api_convert_office_hours($agreement->field_regular_times[LANGUAGE_NONE]);
            for($i=0; $i<7; $i++) {
                $now = time();
                $oneday = 86400;
                $earliestorder = date('Y-m-d', $now + $longestdelay + $oneday);
                $strtotime = strtotime($earliestorder . ' +' . $i . ' days');
                if(!empty($regular_times[date('N', $strtotime)])) {
                    foreach($regular_times[date('N', $strtotime)] as $dateval) {
                        $begin_time = strtotime($earliestorder . ' ' . $dateval['starthours'] . ' +' . $i . ' day');
                        $end_time = strtotime($earliestorder . ' ' . $dateval['endhours'] . ' +' . $i . ' day');
                        $pickuptimes[$agreement->nid][$begin_time . '-' . $end_time] = date('d.m.Y H:i', $begin_time) . ' - ' . date('H:i', $end_time);
                    }
                }
            }
        }
    }
    
    /*
     *  Feld für Abholpunkte
     */
    $form['checkout']['delivery_fs']['pickup_spots'] = array(
        '#type' => 'radios',
        '#title' => t('Pickup spot'),
        '#options' => $pickupspots,
        '#states' => array(
          'visible' => array(   // action to take.
            ':input[name=delivery]' => array('value' => 'pickup_agreement'),
          ),
        ),
    );
    if(count($pickupspots) == 1) {
        $pickupspotskeys = array_keys($pickupspots);
        $form['checkout']['delivery_fs']['pickup_spots']['#default_value'] = $pickupspotskeys[0];
    }

    /*
     *  Felder für Abholzeiten
     */
    if(isset($pickuptimes) && !empty($pickuptimes) && isset($pickupspots) && !empty($pickupspots)) {
        foreach($pickupspots as $pickupspotkey => $pickupspot) {
            $form['checkout']['delivery_fs']['pickup_times']['pickup_time_' . $pickupspotkey] = array(
                '#type' => 'radios',
                '#title' => t('Pickup times'),
                '#options' => $pickuptimes[$pickupspotkey],
                '#states' => array(
                  'visible' => array(   // action to take.
                    ':input[name=delivery]' => array('value' => 'pickup_agreement'),
                    ':input[name=pickup_spots]' => array('value' => $pickupspotkey),
                  ),
                ),
            );
            if(count($pickuptimes[$pickupspotkey]) == 1) {
                $pickuptimeskeys = array_keys($pickuptimes[$pickupspotkey]);
                $form['checkout']['delivery_fs']['pickup_time_' . $pickupspotkey]['#default_value'] = $pickuptimeskeys[0];
            }
        }
    }
    
    /*
     *  Liefervereinbarungen auslesen
     */
    $shippingtimes = array();
    if(isset($minimum_order_values['shipping_agreement']) && !empty($minimum_order_values['shipping_agreement'])) {
        foreach($minimum_order_values['shipping_agreement'] as $agreement_id => $mov) {
            $agreement = node_load($agreement_id);
            for($i=0; $i<7; $i++) {
                $strtotime = strtotime('tomorrow +' . $i . ' day');
                if(!empty($agreement->field_regular_times[LANGUAGE_NONE][date('N', $strtotime) - 1])) {
                    $dateval = $agreement->field_regular_times[LANGUAGE_NONE][date('N', $strtotime) - 1];
                    $begin_time = strtotime('tomorrow ' . substr($dateval['starthours'], 0, -2) . ':' . substr($dateval['starthours'], -2) . ' +' . $i . ' day');
                    $end_time = strtotime('tomorrow ' . substr($dateval['endhours'], 0, -2) . ':' . substr($dateval['endhours'], -2) . ' +' . $i . ' day');
                    $shippingtimes[$begin_time . '-' . $end_time] = date('d.m.Y H:i', $begin_time) . ' - ' . date('H:i', $end_time);
                }
            }
        }
    }

    if(isset($shippingtimes) && !empty($shippingtimes)) {
        /*
         *  Feld für Lieferzeiten
         */
        $form['checkout']['delivery_fs']['shipping_time'] = array(
            '#type' => 'radios',
            '#title' => t('Shipping times'),
            '#options' => $shippingtimes,
            '#states' => array(
              'visible' => array(   // action to take.
                ':input[name=delivery]' => array('value' => 'shipping_agreement'),
              ),
            ),
        );
        if(count($shippingtimes) == 1) {
            $shippingtimeskeys = array_keys($shippingtimes);
            $form['checkout']['delivery_fs']['shipping_time']['#default_value'] = $shippingtimeskeys[0];
        }
        
        /*
         *  Feld für Lieferadresse
         */
        $form['checkout']['delivery_fs']['shipping_address_name'] = array(
            '#type' => 'textfield',
            '#title' => t('Your name'),
            '#states' => array(
              'visible' => array(   // action to take.
                ':input[name=delivery]' => array('value' => 'shipping_agreement'),
                ':input[name=shipping_time]' => array('checked' => TRUE),
              ),
            ),
        );
        $form['checkout']['delivery_fs']['shipping_address_street'] = array(
            '#type' => 'textfield',
            '#title' => t('Street'),
            '#states' => array(
              'visible' => array(   // action to take.
                ':input[name=delivery]' => array('value' => 'shipping_agreement'),
                ':input[name=shipping_time]' => array('checked' => TRUE),
              ),
            ),
        );
        $form['checkout']['delivery_fs']['shipping_address_zip'] = array(
            '#type' => 'textfield',
            '#title' => t('Postal code'),
            '#states' => array(
              'visible' => array(   // action to take.
                ':input[name=delivery]' => array('value' => 'shipping_agreement'),
                ':input[name=shipping_time]' => array('checked' => TRUE),
              ),
            ),
        );
        $form['checkout']['delivery_fs']['shipping_address_city'] = array(
            '#type' => 'textfield',
            '#title' => t('Locality'),
            '#states' => array(
              'visible' => array(   // action to take.
                ':input[name=delivery]' => array('value' => 'shipping_agreement'),
                ':input[name=shipping_time]' => array('checked' => TRUE),
              ),
            ),
        );
    }
    
    /*
     *  Feld für Zahlungsart
     */
    $form['checkout']['payment_fs']['paymenttypes'] = array(
        '#type' => 'radios',
        '#title' => t('Payment types'),
        '#title_display' => 'none',
        '#options' => array(
            'bt' => t('Bank Transfer'),
            'ibt' => t('Instant Bank Transfer'),
            'pp' => t('PayPal'),
        ),
    );
    
    /*
     *  Feld für IBAN
     */
    $form['checkout']['payment_fs']['iban'] = array(
        '#type' => 'textfield',
        '#title' => t('IBAN'),
        '#states' => array(
          'visible' => array(   // action to take.
            ':input[name=paymenttypes]' => array('value' => 'bt'),
          ),
        ),
    );
    
    /*
     *  Feld für Rechnungsadresse
     */
    $form['checkout']['payment_fs']['billing_address_name'] = array(
        '#type' => 'textfield',
        '#title' => t('Your name'),
        '#states' => array(
          'visible' => array(   // action to take.
            ':input[name=paymenttypes]' => array('checked' => TRUE),
          ),
        ),
    );
    
    $form['checkout']['payment_fs']['billing_address_street'] = array(
        '#type' => 'textfield',
        '#title' => t('Street'),
        '#states' => array(
          'visible' => array(   // action to take.
            ':input[name=paymenttypes]' => array('checked' => TRUE),
          ),
        ),
    );
    
    $form['checkout']['payment_fs']['billing_address_zip'] = array(
        '#type' => 'textfield',
        '#title' => t('Postal code'),
        '#states' => array(
          'visible' => array(   // action to take.
            ':input[name=paymenttypes]' => array('checked' => TRUE),
          ),
        ),
    );
    
    $form['checkout']['payment_fs']['billing_address_city'] = array(
        '#type' => 'textfield',
        '#title' => t('Locality'),
        '#states' => array(
          'visible' => array(   // action to take.
            ':input[name=paymenttypes]' => array('checked' => TRUE),
          ),
        ),
    );
    
    /*
     *  Submit
     */
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Purchase now'),
    );
    return $form;
}

function rm_cart_checkout_validate($form, &$form_state) {
    //Basic validation: make sure all sequentially required fields are filled out
    if(empty($form_state['values']['paymenttypes'])) {
        form_set_error('paymenttypes', t('Please select a payment type'));
    }
    else {
        switch($form_state['values']['paymenttypes']) {
            case "bt":
                if(empty($form_state['values']['iban'])) form_set_error('iban', t('Please enter your IBAN'));
            default:
                if(empty($form_state['values']['billing_address_name'])) form_set_error('billing_address_name', t('Please enter the name for your billing address'));
                if(empty($form_state['values']['billing_address_street'])) form_set_error('billing_address_street', t('Please enter the street of your billing address'));
                if(empty($form_state['values']['billing_address_zip'])) form_set_error('billing_address_zip', t('Please enter the zipcode of your billing address'));
                if(empty($form_state['values']['billing_address_city'])) form_set_error('billing_address_city', t('Please enter the city of your billing address'));
                break;
        }
    }
    
    
    if(empty($form_state['values']['delivery'])) {
        form_set_error('delivery', t('Please select a delivery type'));
    }
    else {
        switch($form_state['values']['delivery']) {
            case "shipping_agreement":
                break;
            case "pickup_agreement":
                break;
            case "dispatch_agreement":
                //TBD
                break;
        }
    }
}

function rm_cart_checkout_submit($form, &$form_state) {
    var_dump($form_state['values']); die();
}