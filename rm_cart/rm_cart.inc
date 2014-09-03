<?php

function rm_cart_checkout($form, &$form_state, $account, $deliverytype) {
    global $user;
    
    $_SESSION['chosen_seller'] = $account->uid;
    
    $one_click_setting = rm_cart_get_one_click_setting($user->uid, $account->uid);

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
        '#options' => array(
            'shipping_agreement' => 'Lieferung',
            'pickup_agreement' => 'Selbstabholung',
            'dispatch_agreement' => 'Versand per DHL',
        ),
        '#default_value' => $deliverytype,
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