<?php

function rm_seller_offermanagement() {
    //pass to tpl
    return theme('rm_seller_theme_offermanagement', array(
        'vars' => array(
            'offers' => rm_shop_get_structured_seller_offers(29),
        ),
    ));
}

function rm_seller_editprofile($form, &$form_state) {
    $form = array();
    return $form;
}