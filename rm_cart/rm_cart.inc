<?php

function rm_cart_checkout($form, &$form_state, $account) {
    global $user;
    $_SESSION['chosen_seller'] = $account->uid;
    $form = array();
    $form['checkout'] = array();
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
    return $form;
}