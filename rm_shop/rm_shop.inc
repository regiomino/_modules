<?php
/**
 * Created by PhpStorm.
 * User: Martin
 * Date: 05.08.14
 * Time: 09:34
 */


function rm_shop_get_shops($city, $zipcode, $street) {

    drupal_add_js('https://maps.googleapis.com/maps/api/js?sensor=false', 'external');
    drupal_add_js(drupal_get_path('module', 'rm_shop') . '/js/map.js');

    $allagreements = rm_api_get_nodes_by_properties(array('shipping_agreement', 'dispatch_agreement', 'pickup_agreement', 'payment_agreement'), 1);

    $alluids = array();
    foreach($allagreements as $agreementobject) {
        $alluids[$agreementobject->uid][$agreementobject->type] = $agreementobject;
    }

    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'node');
    $query->entityCondition('bundle', array('seller_profile'));
    $query->propertyCondition('status', 1);
    $query->propertyCondition('uid', array_keys($alluids));
    $result = $query->execute();

    $shops = entity_load('node', array_keys($result['node']));

    foreach($shops as $nid => $shop) {
        $shops[$nid]->agreements = $alluids[$shop->uid];
    }

    //$shops = '<p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p><p>nothing here yet</p>';

    return theme('rm_shop_theme_shops', array(
        'vars' => array(
            'title' => t('Vendors in @region', array('@region' => arg(1))),
            'shops' => $shops,
        ),
    ));
}

function rm_shop_jobs() {

    return	theme('rm_shop_theme_jobs', array(
        'vars' => array(
            'jobs' => 'jobs',
        ),
    ));
}

function rm_shop_get_in_touch() {

    $form = drupal_get_form('rm_shop_contact_form');

    return	theme('rm_shop_theme_get_in_touch', array(
        'vars' => array(
            'contactform' => $form,
            'faq' => 'faq',
        ),
    ));
}

function rm_shop_contact_form($form, &$form_state) {
    $form = array();
    global $user;
    $form['name'] = array(
        '#type' => 'textfield',
        '#title' => t('Your name'),
        '#required' => TRUE,
    );
    if($user->uid > 0) {
        $userobject = user_load($user->uid);
        $form['name']['#default_value'] = $userobject->field_first_name[LANGUAGE_NONE][0]['value'] . ' ' . $userobject->field_last_name[LANGUAGE_NONE][0]['value'];
    }
    $form['email'] = array(
        '#type' => 'textfield',
        '#title' => t('Your E-Mail'),
        '#required' => TRUE,
    );
    if(!empty($user->mail)) $form['email']['#default_value'] = $user->mail;
    $form['body'] = array(
        '#type' => 'textarea',
        '#title' => t('Your inquiry'),
        '#required' => TRUE,
    );
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Send'),
    );
    return $form;
}

function rm_shop_contact_form_validate($form, &$form_state) {
    if(!filter_var($form_state['values']['email'], FILTER_VALIDATE_EMAIL)) form_set_error('email', t('Please enter a correct email address'));
}

function rm_shop_contact_form_submit($form, &$form_state) {
    drupal_mail('rm_shop', 'contact_form', 'system@regiomino.de', NULL, array('email' => $form_state['values']['email'], 'body' => $form_state['values']['body'], 'name' => $form_state['values']['name']), variable_get('site_mail', 'support@regiomino.de'));
}

function rm_shop_mail($key, &$message, $params) {
    global $base_url;
    global $base_path;
    switch($key) {
        //switching on $key lets you create variations of the email based on the $key parameter
        case 'contact_form':
            $message['subject'] = 'Anfrage über das Regiomino Kontaktformular';
            $message['headers']['Reply-To'] = $params['email'];
            $message['body'][] = '<p>Von: ' . $params['name'] . '</p><p>E-Mail: ' . $params['email'] . '</p><p>Anfrage:</p>' . $params['body'];
            break;
    }
}