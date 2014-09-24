<?php

function rm_cart_checkout($form, &$form_state, $account) {
    global $user;
    
    rm_user_enforce_login(t('Please log in with your commercial account before you continue'), 'warning');
    
    $_SESSION['chosen_seller'] = $account->uid;
    
    $one_click_setting = rm_cart_get_one_click_setting($user->uid, $account->uid);
    
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
    
    $shops = rm_shop_get_shop_agreements($user->uid, $zipcode, $account->uid);
    $shops_keys = array_keys($shops);
    $shop = $shops[$shops_keys[0]];

    $form = array();
    $form['checkout'] = array();
    $form['checkout']['oneclick'] = array(
        '#type' => 'value',
        '#value' => (!empty($one_click_setting)) ? $one_click_setting->nid : -1,
    );
    $form['checkout']['delivery'] = array(
        '#type' => 'radios',
        '#title' => t('Delivery option'),
        '#options' => $deliverytypes,
    );
    
    
    //Pickup agreement
    $pickupspots = array();
    $pickuptimes = array();
    if(isset($minimum_order_values['pickup_agreement']) && !empty($minimum_order_values['pickup_agreement'])) {
        foreach($minimum_order_values['pickup_agreement'] as $agreement_id => $mov) {
            $agreement = node_load($agreement_id);
            $pickupspots[$agreement->nid] = $agreement->field_address[LANGUAGE_NONE][0]['locality'] . ', ' . $agreement->field_address[LANGUAGE_NONE][0]['thoroughfare'];
            for($i=0; $i<7; $i++) {
                $strtotime = strtotime('tomorrow +' . $i . ' day');
                if(!empty($agreement->field_regular_times[LANGUAGE_NONE][date('N', $strtotime) - 1])) {
                    $dateval = $agreement->field_regular_times[LANGUAGE_NONE][date('N', $strtotime) - 1];
                    $begin_time = strtotime('tomorrow ' . substr($dateval['starthours'], 0, -2) . ':' . substr($dateval['starthours'], -2) . ' +' . $i . ' day');
                    $end_time = strtotime('tomorrow ' . substr($dateval['endhours'], 0, -2) . ':' . substr($dateval['endhours'], -2) . ' +' . $i . ' day');
                    $pickuptimes[$agreement->nid][$begin_time . '-' . $end_time] = date('d.m.Y H:i', $begin_time) . ' - ' . date('H:i', $end_time);
                }
            }
        }
    }
    
    //Pickup spots
    $form['checkout']['pickup_spots'] = array(
        '#type' => 'radios',
        '#title' => t('Pickup spot'),
        '#options' => $pickupspots,
        '#states' => array(
          'visible' => array(   // action to take.
            ':input[name=delivery]' => array('value' => 'pickup_agreement'),
          ),
        ),
    );

    //Pickup times
    if(isset($pickuptimes) && !empty($pickuptimes) && isset($pickupspots) && !empty($pickupspots)) {
        foreach($pickupspots as $pickupspotkey => $pickupspot) {
            $form['checkout']['pickup_time_' . $pickupspotkey] = array(
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
        }
    }
    
    //Shipping agreement
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

    //Shipping times
    if(isset($shippingtimes) && !empty($shippingtimes)) {
        $form['checkout']['shipping_time'] = array(
            '#type' => 'radios',
            '#title' => t('Shipping times'),
            '#options' => $shippingtimes,
            '#states' => array(
              'visible' => array(   // action to take.
                ':input[name=delivery]' => array('value' => 'shipping_agreement'),
              ),
            ),
        );
        
        $form['checkout']['shipping_address_name'] = array(
            '#type' => 'textfield',
            '#title' => t('Your name'),
            '#states' => array(
              'visible' => array(   // action to take.
                ':input[name=delivery]' => array('value' => 'shipping_agreement'),
                ':input[name=shipping_time]' => array('checked' => TRUE),
              ),
            ),
        );
        
        $form['checkout']['shipping_address_street'] = array(
            '#type' => 'textfield',
            '#title' => t('Street'),
            '#states' => array(
              'visible' => array(   // action to take.
                ':input[name=delivery]' => array('value' => 'shipping_agreement'),
                ':input[name=shipping_time]' => array('checked' => TRUE),
              ),
            ),
        );
        
        $form['checkout']['shipping_address_zip'] = array(
            '#type' => 'textfield',
            '#title' => t('Postal code'),
            '#states' => array(
              'visible' => array(   // action to take.
                ':input[name=delivery]' => array('value' => 'shipping_agreement'),
                ':input[name=shipping_time]' => array('checked' => TRUE),
              ),
            ),
        );
        
        $form['checkout']['shipping_address_city'] = array(
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
    
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Purchase now'),
    );
    return $form;
}

function rm_cart_checkout_submit($form, &$form_state) {
    var_dump($form_state['values']); die();
}