<?php

function rm_cart_checkout($form, &$form_state) {
    global $user;
    $userobject = user_load();
    $form = array();
    $form['checkout'] = array();
    return $form;
}