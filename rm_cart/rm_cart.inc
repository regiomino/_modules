<?php

function rm_cart_checkout($form, &$form_state, $account) {
    global $user;
    
    $_SESSION['chosen_seller'] = $account->uid;
    
    $query = new EntityFieldQuery();
    $tmp = $query
        ->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', array('one_click_setting'))
        ->propertyCondition('status', 1)
        ->propertyCondition('uid', $user->uid)
        ->fieldCondition('field_user_reference', 'target_id', $account->uid)
        ->execute();
    $one_click_settings = array();
    if(isset($tmp['node'])) {
        $tmpkeys = array_keys($tmp['node']);
        $one_click_settings = entity_load('node', $tmpkeys);
    }
    $one_click_setting = array();
    if(!empty($one_click_settings)) {
        $one_click_setting = $one_click_settings[$tmpkeys[0]];
    }
    
    $form = array();
    $form['checkout'] = array();
    $form['checkout']['oneclick'] = array(
        '#type' => 'value',
        '#value' => (!empty($one_click_setting)) ? $one_click_setting->nid : -1,
    );
    $form['checkout']['delivery'] = array(
        '#type' => 'select',
        '#title' => t('Delivery option'),
        '#description' => t('Please select a delivery option'),
        '#options' => array(
            'shipping_agreement' => 'Lieferung',
            'pickup_agreement' => 'Selbstabholung',
            'dispatch_agreement' => 'Versand per DHL',
        ),
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