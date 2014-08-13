<?php

function rm_seller_offermanagement() {
    //pass to tpl
    return theme('rm_seller_theme_offermanagement', array(
        'vars' => array(
            'title' => 'test',
        ),
    ));
}

function rm_seller_editprofile($form, &$form_state) {
    $form = array();
    return $form;
}