<?php

function rm_cart_checkout($form, &$form_state, $account) {
    global $user;
    
    $_SESSION['chosen_seller'] = $account->uid;
    
    $one_click_setting = rm_cart_get_one_click_setting($user->uid, $account->uid);
    
    $minimum_order_values = array();
    $deliverytypes = array();
    if(isset($_SESSION['regionselect']['zip'])) $minimum_order_values = rm_cart_get_minimum_order_values($account->uid, $_SESSION['regionselect']['zip']);
    $carttotal = rm_cart_get_cart_total($account->uid);
    if(isset($minimum_order_values)) {
        foreach($minimum_order_values as $type => $mov) {
            if($carttotal >= $mov) $deliverytypes[$type] = node_type_get_name($type);
        }
    }
    

    $form = array();
    $form['checkout'] = array();
    $form['checkout']['oneclick'] = array(
        '#type' => 'value',
        '#value' => (!empty($one_click_setting)) ? $one_click_setting->nid : -1,
    );
    $form['checkout']['delivery'] = array(
        '#type' => 'radios',
        '#title' => t('Delivery option'),
        '#description' => t('Please select a delivery option'),
        '#options' => $deliverytypes,
    );
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Purchase now'),
    );
    return $form;
}

function rm_cart_checkout_submit($form, &$form_state) {
    var_dump($form_state['values']['oneclick']); die();
}