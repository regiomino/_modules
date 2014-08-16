<?php
/**
 * Created by PhpStorm.
 * User: Martin
 * Date: 05.08.14
 * Time: 09:34
 */


function rm_shop_get_shops($city, $zipcode, $street) {

    global $user;

    drupal_add_js('https://maps.googleapis.com/maps/api/js?sensor=false', 'external');
    drupal_add_js(drupal_get_path('module', 'rm_shop') . '/js/rm_shop.js');

    //Get all shipping agreements that match the given zipcode
    $query = new EntityFieldQuery();
    $tmp = $query
        ->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', array('shipping_agreement'))
        ->propertyCondition('status', 1)
        ->fieldCondition('field_shipping_zipcodes', 'value', $zipcode)
        ->fieldCondition('field_user_reference', 'target_id', ($user->uid > 0) ? array(0, $user->uid) : $user->uid)
        ->execute();
    $shippingagreements = array();
    if(isset($tmp['node'])) $shippingagreements = entity_load('node', array_keys($tmp['node']));
    
    //Get all dispatch, pickup and payment agreements
    $query = new EntityFieldQuery();
    $tmp = $query
        ->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', array('dispatch_agreement', 'pickup_agreement', 'payment_agreement'))
        ->propertyCondition('status', 1)
        ->fieldCondition('field_user_reference', 'target_id', ($user->uid > 0) ? array(0, $user->uid) : $user->uid)
        ->execute();

    $otheragreements = entity_load('node', array_keys($tmp['node']));

    //Merge all
    $allagreements = array_merge($otheragreements, $shippingagreements);

    //Extract all uids from agreements
    $alluids = array();
    foreach($allagreements as $agreementobject) {
        //if an agreement from the same seller of the same type already exists, then check if the current one is more specific or more general
        if(!empty($alluids[$agreementobject->uid][$agreementobject->type])) {
            //step through the existing entries -> should be only one
            foreach($alluids[$agreementobject->uid][$agreementobject->type] as $user_reference_key => $unimportant) {
                //if the already written key is smaller than the current one, this means the current one is more specific
                if($user_reference_key < $agreementobject->field_user_reference[LANGUAGE_NONE][0]['target_id']) {
                    //delete the more general one
                    unset($alluids[$agreementobject->uid][$agreementobject->type][$user_reference_key]);
                }
            }
        }
        //write the current key
        $alluids[$agreementobject->uid][$agreementobject->type][$agreementobject->field_user_reference[LANGUAGE_NONE][0]['target_id']][] = $agreementobject;
    }
    
    $shops = array();
    
    if(!empty($alluids)) {
        //Filter out all uids that do not have active trading units
        $alluidkeys = array_keys($alluids);
        $relevantuids = rm_shop_filter_sellers_with_active_offers($alluidkeys);#
        
        //then select all seller profiles that match the relevant uids
        if(!empty($relevantuids)) {
            $query = new EntityFieldQuery();
            $query->entityCondition('entity_type', 'node');
            $query->entityCondition('bundle', array('seller_profile'));
            $query->propertyCondition('status', 1);
            $query->propertyCondition('uid', $relevantuids);
            $query->pager(15);
            $result = $query->execute();
            if(isset($result['node'])) {
                $shops = entity_load('node', array_keys($result['node']));
            }
        }
    }
    //add all agreement infos to the shop profile
    foreach($shops as $nid => $shop) {
        $shops[$nid]->agreements = $alluids[$shop->uid];
    }

    //pass to tpl
    return theme('rm_shop_theme_shops', array(
        'vars' => array(
            'title' => t('Vendors in @region', array('@region' => arg(1))),
            'shops' => $shops,
            'pager' => theme('pager'),
        ),
    ));
}

function rm_shop_filter_sellers_with_active_offers($uids) {
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'node');
    $query->entityCondition('bundle', array('trading_unit'));
    $query->propertyCondition('status', 1);
    $query->propertyCondition('uid', $uids);
    $result = $query->execute();

    $intersecting_uids = array();
    
    if(isset($result['node'])) {
        $tradingunitowners = array();
        $alltradingunits = entity_load('node', array_keys($result['node']));
        foreach($alltradingunits as $nid => $trading_unit_object) {
            $tradingunitowners[$trading_unit_object->uid] = TRUE;
        }
        $tmp = array_intersect(array_keys($tradingunitowners), $uids);
        
        $intersecting_uids = array_values($tmp);
    }
    
    return $intersecting_uids;
}

function rm_shop_jobs() {
    return	theme('rm_shop_theme_jobs', array(
        'vars' => array(
            'jobs' => rm_api_get_nodes_by_properties(array('job'), 1, -1, -1, -1, -1, -1, 'DESC'),
        ),
    ));
}

function rm_shop_sitemap() {
    $nodes = rm_api_get_nodes_by_properties(array('simple_page', 'faq', 'customer_profile', 'job', 'seller_profile', 'media_object', 'press_report', 'labels'), 1);
    
    $groupednodes = array();
    foreach($nodes as $nid => $nodeobject) {
        $groupednodes[$nodeobject->type][] = $nodeobject;
    }
    
    return	theme('rm_shop_theme_sitemap', array(
        'vars' => array(
            'sitemap' => $groupednodes,
        ),
    ));
}

function rm_shop_get_in_touch() {

    $form = drupal_get_form('rm_shop_contact_form');

    return	theme('rm_shop_theme_get_in_touch', array(
        'vars' => array(
            'contactform' => $form,
            'faq' => rm_api_get_nodes_by_properties(array('faq'), 1, -1, -1, -1, -1, -1, 'DESC'),
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
    drupal_set_message(t('Thank you very much. We have received your message and will get back to you as soon as possible.'), 'status');
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