<?php
/**
 * Created by PhpStorm.
 * User: Martin
 * Date: 05.08.14
 * Time: 09:34
 */

function rm_shop_suggest() {
    drupal_goto('kontakt', array('query' => array('suggest' => 1)));
}

function rm_shop_get_shops($zipcode) {

    $_SESSION['regionselect'] = array(
        'zip' => $zipcode,
    );

    global $user;
    //rm_shop_get_filteredshops();
    //map_center
    //map_bounds_sw
    //map_bounds_ne
    //seller_name
    //seller_type
    //delivery_options
    //payment_types
    $queryparams = drupal_get_query_parameters();
        
    //Map Center
    if(empty($queryparams['map_center'])) {
        $opengeodbinfo = rm_api_opengeodb_queryinfobyzip($zipcode);
        drupal_add_js(array('rm_shop' => array('map_center' => $opengeodbinfo['main_region']['lon'] . ',' . $opengeodbinfo['main_region']['lat'])), 'setting');
    }
    else {
        drupal_add_js(array('rm_shop' => array('map_center' => $queryparams['map_center'])), 'setting');
    }
    
    //Map Bounds Southwest
    if(empty($queryparams['map_bounds_sw'])) {
        drupal_add_js(array('rm_shop' => array('map_bounds_sw' => '')), 'setting');
    }
    else {
        drupal_add_js(array('rm_shop' => array('map_bounds_sw' => $queryparams['map_bounds_sw'])), 'setting');
    }
    
    //Map Bounds Northeast
    if(empty($queryparams['map_bounds_ne'])) {
        drupal_add_js(array('rm_shop' => array('map_bounds_ne' => '')), 'setting');
    }
    else {
        drupal_add_js(array('rm_shop' => array('map_bounds_ne' => $queryparams['map_bounds_ne'])), 'setting');
    }
    
    //Seller name
    if(empty($queryparams['seller_name'])) {
        drupal_add_js(array('rm_shop' => array('seller_name' => '')), 'setting');
    }
    else {
        drupal_add_js(array('rm_shop' => array('seller_name' => $queryparams['seller_name'])), 'setting');
    }
    
    //Seller type
    if(empty($queryparams['seller_type'])) {
        drupal_add_js(array('rm_shop' => array('seller_type' => '')), 'setting');
    }
    else {
        drupal_add_js(array('rm_shop' => array('seller_type' => $queryparams['seller_type'])), 'setting');
    }
    
    //Delivery options
    if(empty($queryparams['delivery_options'])) {
        drupal_add_js(array('rm_shop' => array('delivery_options' => '')), 'setting');
    }
    else {
        drupal_add_js(array('rm_shop' => array('delivery_options' => $queryparams['delivery_options'])), 'setting');
    }
    
    //Payment types
    if(empty($queryparams['payment_types'])) {
        drupal_add_js(array('rm_shop' => array('payment_types' => '')), 'setting');
    }
    else {
        drupal_add_js(array('rm_shop' => array('payment_types' => $queryparams['payment_types'])), 'setting');
    }
    
    

    $shops = rm_shop_get_shop_agreements($user->uid, $zipcode, NULL);

    //pass to tpl
    return theme('rm_shop_theme_shops', array(
        'vars' => array(
            'title' => t('Vendors in @region', array('@region' => arg(1))),
            'shops' => $shops,
            'pager' => theme('pager'),
        ),
    ));
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