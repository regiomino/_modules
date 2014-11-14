<?php

function rm_cart_checkout_confirm($form, &$form_state, $order_number) {
    global $user;
    $form = array();
    $form['order']['order_number'] = array(
        '#type' => 'value',
        '#value' => $order_number,
    );
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Purchase now'),
    );
    return $form;
}

function rm_cart_checkout_confirm_submit($form, &$form_state) {
    rm_cart_change_order_status($form_state['values']['order_number'], 'completed');
    //Perform payment
    $order_items = rm_cart_get_order($form_state['values']['order_number']);
    $order_items_keys = array_keys($order_items);
    $order_item = $order_items[$order_items_keys[0]];
    $userobject = user_load($order_item->uid);

    $shop = rm_shop_get_shop($order_item->field_user_reference[LANGUAGE_NONE][0]['target_id']);
    
    drupal_mail(
        'rm_cart',
        'incoming_order',
        $userobject->mail,
        NULL,
        array(
            'order_number' => $form_state['values']['order_number'],
            'user_gender' => $userobject->field_gender[LANGUAGE_NONE][0]['value'],
            'user_first_name' => $userobject->field_first_name[LANGUAGE_NONE][0]['value'],
            'user_last_name' => $userobject->field_last_name[LANGUAGE_NONE][0]['value'],
            'order_items' => $order_items,
            'billing_address' => $order_item->field_billingaddress[LANGUAGE_NONE][0],
            'shipping_address' => $order_item->field_address[LANGUAGE_NONE][0],
            'delivery_type' => $order_item->field_deliverytype[LANGUAGE_NONE][0]['value'],
            'payment_type' => $order_item->field_paymenttype[LANGUAGE_NONE][0]['value'],
            'pickup_agreement' => $order_item->field_pickup_agreement_ref[LANGUAGE_NONE][0]['target_id'],
            'delivery_range_from' => $order_item->field_delivery_range_from[LANGUAGE_NONE][0]['value'],
            'delivery_range_to' => $order_item->field_delivery_range_to[LANGUAGE_NONE][0]['value'],
            'seller_title' => $shop->title,
            'seller_object' => $shop,
            'seller_user' => user_load($shop->uid)
        ),
        variable_get('site_mail', 'support@regiomino.de')
    );

    switch($order_item->field_paymenttype[LANGUAGE_NONE][0]['value']) {
        case 'cash':
        case 'invoice':
            global $base_url;
            global $base_path;
            rm_cart_successfulpayment($form_state['values']['order_number'], 'external', 0);
            drupal_goto('thankyou/' . $form_state['values']['order_number']);
            break;
        case 'pp':
            global $base_url;
            global $base_path;
            $return_url = $base_url . $base_path . 'thankyou/' . $form_state['values']['order_number'];
            $cancel_return = $base_url . $base_path . 'order/' . $form_state['values']['order_number'];
            $notify_url = $base_url . $base_path . 'internal/confirmation';
            $cmd = '_cart';
            $upload = 1;
            $currency = 'EUR';
            $image = $base_url . $base_path . drupal_get_path('theme', 'rmtwo') . '/images/paypallogo.png';
            $paypal_email = variable_get('regiomino_payment_paypal_account', 'support@regiomino.de');
            $url = 'https://www.paypal.com/cgi-bin/webscr';
            $options = array(
                'query' => array(
                    'cmd' => $cmd,
                    'lc' => 'DE',
                    'upload' => $upload,
                    'business' => $paypal_email,
                    //'amount' => $item_amount,
                    'return' => $return_url,
                    'cancel_return' => $cancel_return,
                    'notify_url' => $notify_url,
                    'currency_code' => $currency,
                    'no_shipping' => 1,
                    'image_url' => $image,
                    //'first_name' => $billingaddressitems[0]['first_name'],
                    'last_name' => $order_item->field_billingaddress[LANGUAGE_NONE][0]['name_line'],
                    'address1' => $order_item->field_billingaddress[LANGUAGE_NONE][0]['thoroughfare'],
                    //'address2' => $billingaddressitems[0]['premise'],
                    'city' => $order_item->field_billingaddress[LANGUAGE_NONE][0]['locality'],
                    'zip' => $order_item->field_billingaddress[LANGUAGE_NONE][0]['postal_code'],
                    'custom' => $form_state['values']['order_number'],
                    'invoice' => $form_state['values']['order_number'],
                ),
                'https' => TRUE,
                'external' => TRUE,
            );
            
            //Add cart items to query
            $counter = 1;
            foreach($order_items as $order_item) {
                $options['query']['item_name_' . $counter] = utf8_decode($order_item->title);
                $options['query']['item_number_' . $counter] = $order_item->nid;
                $options['query']['amount_' . $counter] = round($order_item->field_tu_price[LANGUAGE_NONE][0]['value'] / 100 * (100 + $order_item->field_tu_vat[LANGUAGE_NONE][0]['value']), 2);
                $options['query']['quantity_' . $counter] = $order_item->field_order_amount[LANGUAGE_NONE][0]['value'];
                $counter++;
                //Sum up costs for shipping
                if(isset($order_item->field_tu_deposit[LANGUAGE_NONE][0]['value']) && $order_item->field_tu_deposit[LANGUAGE_NONE][0]['value'] > 0) {
                    $options['query']['item_name_' . $counter] = t('Deposit');
                    $options['query']['item_number_' . $counter] = '0';
                    $options['query']['amount_' . $counter] = round($order_item->field_tu_deposit[LANGUAGE_NONE][0]['value'], 2);
                    $options['query']['quantity_' . $counter] = 1;
                    $counter++;
                }
            }
            
            

            $payment_goto = $url;
            $payment_options = $options;
            
            break;
        case 'ibt':
        
            global $base_url;
            global $base_path;
            $return_url = $base_url . $base_path . 'thankyou/' . $form_state['values']['order_number'];
            $error_return = $base_url . $base_path . 'order/' . $form_state['values']['order_number'];

            $pw = variable_get('regiomino_payment_novalnet_password', 'eb1e78328c46506b46a4ac4a1e378b91');

            #Verschlüsselung
            
            $vendor = variable_get('regiomino_payment_novalnet_vendor', 1852);
            $auth_code_encoded = variable_get('regiomino_payment_novalnet_authcode', 'fXtYibPcVkVy7iJvKQ0WPZ2yK6Mwan');
            $product_encoded = variable_get('regiomino_payment_novalnet_product', 2489);
            $tariff_encoded = variable_get('regiomino_payment_novalnet_tariff', 4115);
            $test_mode_encoded = variable_get('regiomino_payment_novalnet_testmode', 0);
            
            $orderaddup = 0;
            foreach($order_items as $order_item) {
                $orderaddup += round($order_item->field_order_amount[LANGUAGE_NONE][0]['value'] * ($order_item->field_tu_price[LANGUAGE_NONE][0]['value'] / 100 * (100 + $order_item->field_tu_vat[LANGUAGE_NONE][0]['value'])), 2);
                if(isset($order_item->field_tu_deposit[LANGUAGE_NONE][0]['value']) && $order_item->field_tu_deposit[LANGUAGE_NONE][0]['value'] > 0) {
                    $orderaddup += round($order_item->field_tu_deposit[LANGUAGE_NONE][0]['value'], 2);
                }
            }
            
            $auth_code_encoded = rm_cart_novalnet_encode($auth_code_encoded, $pw);
            $product_encoded = rm_cart_novalnet_encode($product_encoded, $pw);
            $tariff_encoded = rm_cart_novalnet_encode($tariff_encoded, $pw);
            $test_mode_encoded = rm_cart_novalnet_encode($test_mode_encoded, $pw);
            $uniqid_encoded = rm_cart_novalnet_encode($form_state['values']['order_number'], $pw);
            $amount_encoded = rm_cart_novalnet_encode($orderaddup*100, $pw);

            #Hashwert-Berechnung
            $hashP = array(
                'product_id' => $product_encoded,
                'tariff' => $tariff_encoded,
                'auth_code' => $auth_code_encoded,
                'amount' => $amount_encoded,
                'test_mode' => $test_mode_encoded,
                'uniqid' => $uniqid_encoded,
            );
            $hash = rm_cart_novalnet_hashIt($hashP, $pw);

            
            /*Die o.g. 6 Parameter (auth_code, product, tariff, amount, test_mode, uniqid) werden auch bei der Antwort von Novalnet entsprechend verschlüsselt, der Hash-Wert seitens Novalnet wird im Parameter hash2 übermittelt. Anhand der Funktion checkHash (siehe unten) kann man feststellen, ob eine Manipulation stattgefunden hat:
            $_REQUEST enthält u.a.:
            $_REQUEST['hash2'],
            $_REQUEST['auth_code'],
            $_REQUEST['product'],
            $_REQUEST['tariff'],
            $_REQUEST['amount'],
            $_REQUEST['test_mode'],
            $_REQUEST['uniqid']
            */
            /*
            #nun check
            if (rm_cart_novalnet_checkHash($_REQUEST, $pw) == true) {}#OK
            else{}#not OK, Daten sind vielleicht manipuliert.
            #Entschlüsselung:
            $auth_code = rm_cart_novalnet_decode($auth_code_encoded, $pw); # oder $auth_code = rm_cart_novalnet_decode($_REQUEST['auth_code'], $pw);
            $product = rm_cart_novalnet_decode($product_encoded, $pw);
            $tariff = rm_cart_novalnet_decode($tariff_encoded, $pw);
            $amount = rm_cart_novalnet_decode($amount_encoded, $pw);
            $test_mode = rm_cart_novalnet_decode($test_mode_encoded, $pw);
            $uniqid = rm_cart_novalnet_decode($uniqid_encoded, $pw);
            */
            
            $url = 'internal/provider';
            global $user;
            $customername = explode(' ', $order_item->field_billingaddress[LANGUAGE_NONE][0]['name_line']);
            if(!isset($customername[1])) {
                $customername[0] = 'na';
                $customername[1] = $order_item->field_billingaddress[LANGUAGE_NONE][0]['name_line'];
            }
            $options = array(
                'query' => array(
                    'utf8' => 1,
                    'vendor' => $vendor,
                    'product' => $product_encoded,
                    'key' => 33,
                    'tariff' => $tariff_encoded,
                    'auth_code' => $auth_code_encoded,
                    'currency' => 'EUR',
                    'amount' => $amount_encoded,
                    'first_name' => $customername[0],
                    'last_name' => $customername[1],
                    'email' => $user->mail,
                    'street' => $order_item->field_billingaddress[LANGUAGE_NONE][0]['thoroughfare'],
                    'search_in_street' => 1,
                    'city' => $order_item->field_billingaddress[LANGUAGE_NONE][0]['locality'],
                    'zip' => $order_item->field_billingaddress[LANGUAGE_NONE][0]['postal_code'],
                    'country_code' => 'DE',
                    'lang' => 'DE',
                    'remote_ip' => ip_address(),
                    'return_url' => $return_url,
                    'return_method' => 'POST',
                    'error_return_url' => $error_return,
                    'error_return_method' => 'POST',
                    'test_mode' => $test_mode_encoded,
                    'user_variable_0' => 'www.regiomino.de',
                    'uniqid' => $uniqid_encoded,
                    'hash' => $hash,
                ),
                'https' => TRUE,
            );
            $payment_goto = $url;
            $payment_options = $options;
            break;
        case 'sepa':
            break;
    }
    drupal_goto($payment_goto, $payment_options);
}

function rm_cart_checkout($form, &$form_state, $account) {
    global $user;
    $userobject = user_load($user->uid);
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
    $form['checkout']['chosen_seller'] = array(
        '#type' => 'value',
        '#value' => $account->uid,
    );
    
    /*
     *  One Click Einstellung in value-Feld speichern um diese nach dem Submit aktualisieren zu können
     *  steht auf -1 wenn keine Einstellung vorhanden ist und neu angelegt werden muss
     */
    $one_click_setting = rm_cart_get_one_click_setting($user->uid, $account->uid);
    //if($one_click_setting) drupal_set_message('Wir haben die Daten von Ihrem letzten Einkauf übernommen, um Ihnen den Kaufprozess zu erleichtern. Bitte prüfen Sie ob die Adresse von uns richtig ausgefüllt wurde.');
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
     *  Abholvereinbarungen auslesen
     */
    $pickupspots = array();
    $pickuptimes = array();
    $pickupdays = array();
    if(isset($minimum_order_values['pickup_agreement']) && !empty($minimum_order_values['pickup_agreement'])) {
        foreach($minimum_order_values['pickup_agreement'] as $agreement_id => $mov) {
            $agreement = node_load($agreement_id);
            
            //Pickup spots festlegen
            $pickupspots[$agreement->nid] = $agreement->field_address[LANGUAGE_NONE][0]['locality'] . ', ' . $agreement->field_address[LANGUAGE_NONE][0]['thoroughfare'];
            $surcharge = 0;
            if(!empty($agreement->field_surcharge[LANGUAGE_NONE][0]['value']) && $agreement->field_surcharge[LANGUAGE_NONE][0]['value'] > 0) $surcharge = $agreement->field_surcharge[LANGUAGE_NONE][0]['value'];
            $pickupspots[$agreement->nid] .= ($surcharge > 0) ? ' (+' . number_format($surcharge, 2, ",", ".") . '€)' : ' (' . t('free of charge') . ')';
            
            $regular_times = rm_api_convert_office_hours($agreement->field_regular_times[LANGUAGE_NONE]);
            for($i=0; $i<7; $i++) {
                $now = time();
                $oneday = 86400;
                $earliestorder = date('Y-m-d', $now + ($longestdelay * 3600) + $oneday);
                $strtotime = strtotime($earliestorder . ' +' . $i . ' days');
                if(!empty($regular_times[date('N', $strtotime)])) {
                    foreach($regular_times[date('N', $strtotime)] as $dateval) {
                        $begin_time = strtotime($earliestorder . ' ' . $dateval['starthours'] . ' +' . $i . ' day');
                        $end_time = strtotime($earliestorder . ' ' . $dateval['endhours'] . ' +' . $i . ' day');
                        $pickupdays[$agreement->nid][date('Ymd', $begin_time)] = t(date('l', $begin_time)) . ', ' . date('d.m.Y', $begin_time);
                        $pickuptimes[$agreement->nid][date('Ymd', $begin_time)][$begin_time . '-' . $end_time] = date('H:i', $begin_time) . ' - ' . date('H:i', $end_time);
                    }
                }
            }
        }
    }
    
    /*
     *  Liefervereinbarungen auslesen
     */
    $shippingtimes = array();
    $shippingdays = array();
    if(isset($minimum_order_values['shipping_agreement']) && !empty($minimum_order_values['shipping_agreement'])) {
        foreach($minimum_order_values['shipping_agreement'] as $agreement_id => $mov) {
            $agreement = node_load($agreement_id);
            for($i=0; $i<7; $i++) {
                $strtotime = strtotime('tomorrow +' . $i . ' day');
                if(!empty($agreement->field_regular_times[LANGUAGE_NONE][date('N', $strtotime) - 1])) {
                    $dateval = $agreement->field_regular_times[LANGUAGE_NONE][date('N', $strtotime) - 1];
                    $begin_time = strtotime('tomorrow ' . substr($dateval['starthours'], 0, -2) . ':' . substr($dateval['starthours'], -2) . ' +' . $i . ' day');
                    $end_time = strtotime('tomorrow ' . substr($dateval['endhours'], 0, -2) . ':' . substr($dateval['endhours'], -2) . ' +' . $i . ' day');
                    $shippingdays[date('Ymd', $begin_time)] = t(date('l', $begin_time)) . ', ' . date('d.m.Y', $begin_time);
                    $shippingtimes[date('Ymd', $begin_time)][$begin_time . '-' . $end_time] = date('H:i', $begin_time) . ' - ' . date('H:i', $end_time);
                }
            }
        }
        $form['checkout']['delivery_fs']['shipping_spots'] = array(
            '#type' => 'value',
            '#value' => $agreement_id,
        );
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
    if(isset($form['checkout']['delivery_fs']['delivery']['#options']['shipping_agreement'])) {
        $shipping_spot_surcharge = node_load($form['checkout']['delivery_fs']['shipping_spots']['#value'])->field_surcharge[LANGUAGE_NONE][0]['value'];
        $form['checkout']['delivery_fs']['delivery']['#options']['shipping_agreement'] .= ($shipping_spot_surcharge > 0) ? '<br><small>(+' . number_format($shipping_spot_surcharge, 2, ",", ".") . '€)</small>' : '<br><small>(' . t('free of charge') . ')</small>';
    }
    if(count($deliverytypes) >= 1) {
        $deliverytypeskeys = array_keys($deliverytypes);
        $form['checkout']['delivery_fs']['delivery']['#default_value'] = $deliverytypeskeys[0];
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
    if(count($pickupspots) >= 1) {
        $pickupspotskeys = array_keys($pickupspots);
        $form['checkout']['delivery_fs']['pickup_spots']['#default_value'] = $pickupspotskeys[0];
    }

    /*
     *  Felder für Abholzeiten
     */
    if(isset($pickupdays) && !empty($pickupdays) && isset($pickupspots) && !empty($pickupspots)) {
        foreach($pickupspots as $pickupspotkey => $pickupspot) {
            $form['checkout']['delivery_fs']['pickup_days']['pickup_day_' . $pickupspotkey] = array(
                '#type' => 'select',
                '#title' => t('Pickup days'),
                '#title_display' => 'none',
                '#options' => $pickupdays[$pickupspotkey],
                '#field_prefix' => '<span class="fa fa-calendar"></span>',
                '#states' => array(
                  'visible' => array(   // action to take.
                    ':input[name=delivery]' => array('value' => 'pickup_agreement'),
                    ':input[name=pickup_spots]' => array('value' => $pickupspotkey),
                  ),
                ),
            );
            if(count($pickupdays[$pickupspotkey]) == 1) {
                $pickupdayskeys = array_keys($pickupdays[$pickupspotkey]);
                $form['checkout']['delivery_fs']['pickup_days']['pickup_day_' . $pickupspotkey]['#default_value'] = $pickupdayskeys[0];
            }
            
            
            foreach($pickupdays[$pickupspotkey] as $pickupdaykey => $pickupday) {
                $form['checkout']['delivery_fs']['pickup_times']['pickup_time_' . $pickupspotkey . '_' . $pickupdaykey] = array(
                    '#type' => 'select',
                    '#title' => t('Pickup times'),
                    '#title_display' => 'none',
                    '#options' => $pickuptimes[$pickupspotkey][$pickupdaykey],
                    '#field_prefix' => '<span class="fa fa-clock-o"></span>',
                    '#states' => array(
                      'visible' => array(   // action to take.
                        ':input[name=delivery]' => array('value' => 'pickup_agreement'),
                        ':input[name=pickup_spots]' => array('value' => $pickupspotkey),
                        ':input[name=pickup_day_' . $pickupspotkey . ']' => array('value' => $pickupdaykey),
                      ),
                    ),
                );
                if(count($pickuptimes[$pickupspotkey][$pickupdaykey]) == 1) {
                    $pickuptimeskeys = array_keys($pickuptimes[$pickupspotkey][$pickupdaykey]);
                    $form['checkout']['delivery_fs']['pickup_times']['pickup_time_' . $pickupspotkey . '_' . $pickupdaykey]['#default_value'] = $pickuptimeskeys[0];
                }
            }
        
        }
    }
    

    if(isset($shippingdays) && !empty($shippingdays)) {
        /*
         *  Feld für Lieferzeiten
         */
        $form['checkout']['delivery_fs']['shipping_days']['shipping_day'] = array(
            '#type' => 'select',
            '#title' => t('Shipping days'),
            '#options' => $shippingdays,
            '#field_prefix' => '<span class="fa fa-calendar"></span>',
            '#title_display' => 'none',
            '#states' => array(
              'visible' => array(   // action to take.
                ':input[name=delivery]' => array('value' => 'shipping_agreement'),
              ),
            ),
        );
        if(count($shippingdays) == 1) {
            $shippingdayskeys = array_keys($shippingdays);
            $form['checkout']['delivery_fs']['shipping_days']['shipping_day']['#default_value'] = $shippingdayskeys[0];
        }
        
        foreach($shippingdays as $shippingdaykey => $shippingday) {
            $form['checkout']['delivery_fs']['shipping_times']['shipping_time_' . $shippingdaykey] = array(
                '#type' => 'select',
                '#title' => t('Shipping times'),
                '#title_display' => 'none',
                '#options' => $shippingtimes[$shippingdaykey],
                '#field_prefix' => '<span class="fa fa-clock-o"></span>',
                '#states' => array(
                  'visible' => array(   // action to take.
                    ':input[name=delivery]' => array('value' => 'shipping_agreement'),
                    ':input[name=shipping_day]' => array('value' => $shippingdaykey),
                  ),
                ),
            );
            if(count($shippingtimes) == 1) {
                $shippingtimeskeys = array_keys($shippingtimes);
                $form['checkout']['delivery_fs']['shipping_times']['shipping_time_' . $shippingdaykey]['#default_value'] = $shippingtimeskeys[0];
            }
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
              ),
            ),
        );
        if($one_click_setting) {
            $form['checkout']['delivery_fs']['shipping_address_name']['#default_value'] = $one_click_setting->field_address[LANGUAGE_NONE][0]['name_line'];
        }
        else {
            $form['checkout']['delivery_fs']['shipping_address_name']['#default_value'] = $userobject->field_first_name[LANGUAGE_NONE][0]['value'] . ' ' . $userobject->field_last_name[LANGUAGE_NONE][0]['value'];
        }
        
        $form['checkout']['delivery_fs']['shipping_address_street'] = array(
            '#type' => 'textfield',
            '#title' => t('Street'),
            '#states' => array(
              'visible' => array(   // action to take.
                ':input[name=delivery]' => array('value' => 'shipping_agreement'),
              ),
            ),
        );
        if($one_click_setting) {
            $form['checkout']['delivery_fs']['shipping_address_street']['#default_value'] = $one_click_setting->field_address[LANGUAGE_NONE][0]['thoroughfare'];
        }
        else {
            $form['checkout']['delivery_fs']['shipping_address_street']['#default_value'] = $userobject->field_address[LANGUAGE_NONE][0]['thoroughfare'];
        }
        
        $form['checkout']['delivery_fs']['shipping_address_zip'] = array(
            '#type' => 'textfield',
            '#title' => t('Postal code'),
            '#states' => array(
              'visible' => array(   // action to take.
                ':input[name=delivery]' => array('value' => 'shipping_agreement'),
              ),
            ),
            '#value' => $_SESSION['regionselect']['zip'],
            '#disabled' => TRUE,
        );
        
        $form['checkout']['delivery_fs']['shipping_address_city'] = array(
            '#type' => 'textfield',
            '#title' => t('Locality'),
            '#states' => array(
              'visible' => array(   // action to take.
                ':input[name=delivery]' => array('value' => 'shipping_agreement'),
              ),
            ),
        );
        if($one_click_setting) {
            $form['checkout']['delivery_fs']['shipping_address_city']['#default_value'] = $one_click_setting->field_address[LANGUAGE_NONE][0]['locality'];
        }
        else {
            $form['checkout']['delivery_fs']['shipping_address_city']['#default_value'] = $userobject->field_address[LANGUAGE_NONE][0]['locality'];
        }
        
    }
    
    /*
     *  Feld für Zahlungsart
     */
    $allpaymenttypes = array(
        'sepa' => t('SEPA'),
        'ibt' => t('Instant Bank Transfer'),
        'pp' => t('PayPal'),
        'cash' => t('Cash'),
        'invoice' => t('Invoice'),
    );
    $availablepaymenttypes = array();
    $shops = rm_shop_get_shop_agreements($user->uid, $zipcode, $account->uid);
    $shop_ids = array_keys($shops);
    $shop = $shops[$shop_ids[0]];
    foreach($shop->agreements['payment_agreement'] as $user_reference => $payment_agreements) {
        foreach($payment_agreements as $payment_agreement) {
            foreach($payment_agreement->field_payment_types[LANGUAGE_NONE] as $field_paymenttype) {
                if($field_paymenttype['value'] == 'prepaid') {
                    //$availablepaymenttypes['sepa'] = $allpaymenttypes['sepa'];
                    $availablepaymenttypes['ibt'] = $allpaymenttypes['ibt'];
                    $availablepaymenttypes['pp'] = $allpaymenttypes['pp'];
                }
                else {
                    $availablepaymenttypes[$field_paymenttype['value']] = $allpaymenttypes[$field_paymenttype['value']];
                }
            }
        }
    }

    $form['checkout']['payment_fs']['paymenttypes'] = array(
        '#type' => 'radios',
        '#title' => t('Payment types'),
        '#title_display' => 'none',
        '#options' => $availablepaymenttypes,
    );
    if($one_click_setting) {
        $form['checkout']['payment_fs']['paymenttypes']['#default_value'] = $one_click_setting->field_paymenttype[LANGUAGE_NONE][0]['value'];
    }
    
    /*
     *  Feld für IBAN
     */
    $form['checkout']['payment_fs']['iban'] = array(
        '#type' => 'textfield',
        '#title' => t('IBAN'),
        '#states' => array(
          'visible' => array(   // action to take.
            ':input[name=paymenttypes]' => array('value' => 'sepa'),
          ),
        ),
    );
    
    /*
     *  Feld für PayPal
     */
    $form['checkout']['payment_fs']['paypal'] = array(
        '#type' => 'textfield',
        '#value' => t('You will be redirected to @paymentprovider after checkout to complete your order', array('@paymentprovider' => 'PayPal')),
        '#disabled' => TRUE,
        '#states' => array(
          'visible' => array(   // action to take.
            ':input[name=paymenttypes]' => array('value' => 'pp'),
          ),
        ),
    );
    
    /*
     *  Feld für Sofortüberweisung
     */
    $form['checkout']['payment_fs']['ibt'] = array(
        '#type' => 'textfield',
        '#value' => t('You will be redirected to @paymentprovider after checkout to complete your order', array('@paymentprovider' => 'sofort.com')),
        '#disabled' => TRUE,
        '#states' => array(
          'visible' => array(   // action to take.
            ':input[name=paymenttypes]' => array('value' => 'ibt'),
          ),
        ),
    );
    
    /*
     *  Feld für Rechnungsadresse
     */
    $form['checkout']['payment_fs']['billing_address_name'] = array(
        '#type' => 'textfield',
        '#title' => t('Your name'),
        // '#states' => array(
          // 'visible' => array(   // action to take.
            // ':input[name=paymenttypes]' => array('checked' => TRUE),
          // ),
        // ),
    );
    if($one_click_setting) {
        $form['checkout']['payment_fs']['billing_address_name']['#default_value'] = $one_click_setting->field_billingaddress[LANGUAGE_NONE][0]['name_line'];
    }
    else {
        $form['checkout']['payment_fs']['billing_address_name']['#default_value'] = $userobject->field_first_name[LANGUAGE_NONE][0]['value'] . ' ' . $userobject->field_last_name[LANGUAGE_NONE][0]['value'];
    }
    
    $form['checkout']['payment_fs']['billing_address_street'] = array(
        '#type' => 'textfield',
        '#title' => t('Street'),
        // '#states' => array(
          // 'visible' => array(   // action to take.
            // ':input[name=paymenttypes]' => array('checked' => TRUE),
          // ),
        // ),
    );
    if($one_click_setting) {
        $form['checkout']['payment_fs']['billing_address_street']['#default_value'] = $one_click_setting->field_billingaddress[LANGUAGE_NONE][0]['thoroughfare'];
    }
    else {
        $form['checkout']['payment_fs']['billing_address_street']['#default_value'] = $userobject->field_address[LANGUAGE_NONE][0]['thoroughfare'];
    }
    
    $form['checkout']['payment_fs']['billing_address_zip'] = array(
        '#type' => 'textfield',
        '#title' => t('Postal code'),
        // '#states' => array(
          // 'visible' => array(   // action to take.
            // ':input[name=paymenttypes]' => array('checked' => TRUE),
          // ),
        // ),
    );
    if($one_click_setting) {
        $form['checkout']['payment_fs']['billing_address_zip']['#default_value'] = $one_click_setting->field_billingaddress[LANGUAGE_NONE][0]['postal_code'];
    }
    else {
        $form['checkout']['payment_fs']['billing_address_zip']['#default_value'] = $userobject->field_address[LANGUAGE_NONE][0]['postal_code'];
    }
    
    $form['checkout']['payment_fs']['billing_address_city'] = array(
        '#type' => 'textfield',
        '#title' => t('Locality'),
        // '#states' => array(
          // 'visible' => array(   // action to take.
            // ':input[name=paymenttypes]' => array('checked' => TRUE),
          // ),
        // ),
    );
    if($one_click_setting) {
        $form['checkout']['payment_fs']['billing_address_city']['#default_value'] = $one_click_setting->field_billingaddress[LANGUAGE_NONE][0]['locality'];
    }
    else {
        $form['checkout']['payment_fs']['billing_address_city']['#default_value'] = $userobject->field_address[LANGUAGE_NONE][0]['locality'];
    }
    
    /*
     *  Submit
     */
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Continue'),
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
            case "sepa":
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
    global $user;
    if($form_state['values']['oneclick'] > 0) {
        //Load oneclick setting
        $oneclick = node_load($form_state['values']['oneclick']);
        //Update values
        $oneclick->field_billingaddress[LANGUAGE_NONE][0]['name_line'] = $form_state['values']['billing_address_name'];
        $oneclick->field_billingaddress[LANGUAGE_NONE][0]['thoroughfare'] = $form_state['values']['billing_address_street'];
        $oneclick->field_billingaddress[LANGUAGE_NONE][0]['postal_code'] = $form_state['values']['billing_address_zip'];
        $oneclick->field_billingaddress[LANGUAGE_NONE][0]['locality'] = $form_state['values']['billing_address_city'];
        $oneclick->field_address[LANGUAGE_NONE][0]['name_line'] = $form_state['values']['shipping_address_name'];
        $oneclick->field_address[LANGUAGE_NONE][0]['thoroughfare'] = $form_state['values']['shipping_address_street'];
        $oneclick->field_address[LANGUAGE_NONE][0]['postal_code'] = $form_state['values']['shipping_address_zip'];
        $oneclick->field_address[LANGUAGE_NONE][0]['locality'] = $form_state['values']['shipping_address_city'];
        $oneclick->field_paymenttype[LANGUAGE_NONE][0]['value'] = $form_state['values']['paymenttypes'];
        //Save oneclick setting
        node_save($oneclick);
    }
    else {
        //Create oneclick setting
        $oneclick = rm_api_create_new_node('one_click_setting', 'Oneclick U:' . $user->uid . ' S:' . $form_state['values']['chosen_seller'], 'de', $user->uid, 1);
        $oneclick->field_user_reference[LANGUAGE_NONE][0]['target_id'] = $form_state['values']['chosen_seller'];
        //Store values
        $oneclick->field_billingaddress[LANGUAGE_NONE][0]['name_line'] = $form_state['values']['billing_address_name'];
        $oneclick->field_billingaddress[LANGUAGE_NONE][0]['thoroughfare'] = $form_state['values']['billing_address_street'];
        $oneclick->field_billingaddress[LANGUAGE_NONE][0]['postal_code'] = $form_state['values']['billing_address_zip'];
        $oneclick->field_billingaddress[LANGUAGE_NONE][0]['locality'] = $form_state['values']['billing_address_city'];
        $oneclick->field_address[LANGUAGE_NONE][0]['name_line'] = $form_state['values']['shipping_address_name'];
        $oneclick->field_address[LANGUAGE_NONE][0]['thoroughfare'] = $form_state['values']['shipping_address_street'];
        $oneclick->field_address[LANGUAGE_NONE][0]['postal_code'] = $form_state['values']['shipping_address_zip'];
        $oneclick->field_address[LANGUAGE_NONE][0]['locality'] = $form_state['values']['shipping_address_city'];
        $oneclick->field_paymenttype[LANGUAGE_NONE][0]['value'] = $form_state['values']['paymenttypes'];
        //Save oneclick setting
        node_save($oneclick);
    }
    $now = time();
    $nowdate = date('d.mY H:i:s', $now);
    $cart_items = rm_cart_get_cart($form_state['values']['chosen_seller'], $user->uid);
    $longestdelay = 0;
    if(!empty($cart_items)) {
        //Determine commission
        $cart_item_keys = array_keys($cart_items);
        $commission = rm_shop_get_commission($user->uid, $cart_items[$cart_item_keys[0]]->field_user_reference[LANGUAGE_NONE][0]['target_id']);
        $order_number = rm_cart_generate_order_number($user->uid, $cart_items[$cart_item_keys[0]]->field_user_reference[LANGUAGE_NONE][0]['target_id'], $now);
        $order_status = 'pending';
        $billing_name = $form_state['values']['billing_address_name'];
        $billing_street = $form_state['values']['billing_address_street'];
        $billing_zip = $form_state['values']['billing_address_zip'];
        $billing_city = $form_state['values']['billing_address_city'];
        $shipping_name = (isset($form_state['values']['shipping_address_name'])) ? $form_state['values']['shipping_address_name'] : '';
        $shipping_street = (isset($form_state['values']['shipping_address_street'])) ? $form_state['values']['shipping_address_street'] : '';
        $shipping_zip = (isset($form_state['values']['shipping_address_zip'])) ? $form_state['values']['shipping_address_zip'] : '';
        $shipping_city = (isset($form_state['values']['shipping_address_city'])) ? $form_state['values']['shipping_address_city'] : '';
        $delivery_type = $form_state['values']['delivery'];
        $payment_type = $form_state['values']['paymenttypes'];
        $iban = $form_state['values']['iban'];
        switch($delivery_type) {
            case 'pickup_agreement':
                $timevals = explode('-', $form_state['values']['pickup_time_' . $form_state['values']['pickup_spots'] . '_' . $form_state['values']['pickup_day_' . $form_state['values']['pickup_spots']]]);
                $delivery_range_from = $timevals[0];
                $delivery_range_to = $timevals[1];
                $pickup_spot = $form_state['values']['pickup_spots'];
                $pickup_spot_surcharge = node_load($pickup_spot)->field_surcharge[LANGUAGE_NONE][0]['value'];
                if($pickup_spot_surcharge > 0) {
                    $order_item = rm_cart_generate_order_surcharge($order_number, $order_status, t('Surcharge for pickup'), 1, $pickup_spot_surcharge, 19, $billing_name, $billing_street, $billing_zip, $billing_city, $shipping_name, $shipping_street, $shipping_zip, $shipping_city, $payment_type, $iban, t('Surcharge for pickup'), $form_state['values']['chosen_seller']);
                }
                break;
            case 'shipping_agreement':
                $timevals = explode('-', $form_state['values']['shipping_time_' . $form_state['values']['shipping_day']]);
                $delivery_range_from = $timevals[0];
                $delivery_range_to = $timevals[1];
                $pickup_spot = 0;
                $shipping_spot_surcharge = node_load($form_state['values']['shipping_spots'])->field_surcharge[LANGUAGE_NONE][0]['value'];
                if($shipping_spot_surcharge > 0) {
                    $order_item = rm_cart_generate_order_surcharge($order_number, $order_status, t('Surcharge for shipping'), 1, $shipping_spot_surcharge, 19, $billing_name, $billing_street, $billing_zip, $billing_city, $shipping_name, $shipping_street, $shipping_zip, $shipping_city, $payment_type, $iban, t('Surcharge for shipping'), $form_state['values']['chosen_seller']);
                }
                break;
        }
        
        foreach($cart_items as $cart_item) {
            $cart_item_tu = node_load($cart_item->field_trading_unit_reference[LANGUAGE_NONE][0]['target_id']);
            $cart_item_variation = node_load($cart_item->field_offer_variation_reference[LANGUAGE_NONE][0]['target_id']);
            $order_amount = $cart_item->field_quantity[LANGUAGE_NONE][0]['value'];
            $tu_amount = $cart_item_tu->field_tu_amount[LANGUAGE_NONE][0]['value'];
            $tu_price = $cart_item_tu->field_tu_price[LANGUAGE_NONE][0]['value'];
            $tu_rrp = $cart_item_tu->field_tu_rrp[LANGUAGE_NONE][0]['value'];
            $tu_deposit = $cart_item_tu->field_tu_deposit[LANGUAGE_NONE][0]['value'];
            $tu_vat = $cart_item_tu->field_tu_vat[LANGUAGE_NONE][0]['value'];
            $tu_packaging = $cart_item_tu->field_tu_packaging[LANGUAGE_NONE][0]['value'];            
            $expiration_date = (isset($cart_item_variation->field_expiration_date[LANGUAGE_NONE][0]['value'])) ? $cart_item_variation->field_expiration_date[LANGUAGE_NONE][0]['value'] : '';
            $use_within = (isset($cart_item_variation->field_use_within[LANGUAGE_NONE][0]['value'])) ? $cart_item_variation->field_use_within[LANGUAGE_NONE][0]['value'] : 0;
            $leadtime = (isset($cart_item_variation->field_leadtime[LANGUAGE_NONE][0]['value'])) ? $cart_item_variation->field_leadtime[LANGUAGE_NONE][0]['value'] : 0;
            $gtin = (isset($cart_item_variation->field_gtin[LANGUAGE_NONE][0]['value'])) ? $cart_item_variation->field_gtin[LANGUAGE_NONE][0]['value'] : '';
            $productunit_first = (isset($cart_item_variation->field_productunit[LANGUAGE_NONE][0]['first'])) ? $cart_item_variation->field_productunit[LANGUAGE_NONE][0]['first'] : '';
            $productunit_second = (isset($cart_item_variation->field_productunit[LANGUAGE_NONE][0]['second'])) ? $cart_item_variation->field_productunit[LANGUAGE_NONE][0]['second'] : '';
            $sku = (isset($cart_item_variation->field_sku[LANGUAGE_NONE][0]['value'])) ? $cart_item_variation->field_sku[LANGUAGE_NONE][0]['value'] : '';
            $description = (isset($cart_item_variation->body[LANGUAGE_NONE][0]['value'])) ? $cart_item_variation->body[LANGUAGE_NONE][0]['value'] : '';
            $offer_desc_reference = $cart_item->field_offer_desc_reference[LANGUAGE_NONE][0]['target_id'];
            $offer_variation_reference = $cart_item->field_offer_variation_reference[LANGUAGE_NONE][0]['target_id'];
            $trading_unit_reference = $cart_item->field_trading_unit_reference[LANGUAGE_NONE][0]['target_id'];
            $seller_reference = $cart_item->field_user_reference[LANGUAGE_NONE][0]['target_id'];
        
            $order_item = rm_cart_generate_order($order_number, $order_status, $cart_item_variation->title, $order_amount, $tu_amount, $tu_price, $tu_rrp, $tu_deposit, $tu_vat, $tu_packaging, $billing_name, $billing_street, $billing_zip, $billing_city, $shipping_name, $shipping_street, $shipping_zip, $shipping_city, $delivery_type, $pickup_spot, $delivery_range_from, $delivery_range_to, $payment_type, $iban, $commission, $expiration_date, $use_within, $leadtime, $gtin, $productunit_first, $productunit_second, $sku, $description, $offer_desc_reference, $offer_variation_reference, $trading_unit_reference,$seller_reference, $uid = NULL);
        }
        drupal_goto('order/' . $order_number);
    }
    else {
        drupal_set_message(t('Your cart is empty', 'error'));
        drupal_goto();
    }
    
}