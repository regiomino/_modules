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

function rm_shop_about_us() {
    return theme('rm_shop_theme_aboutus', array(
        'vars' => array(),
    ));
}

function rm_shop_get_shops($zipcode) {

    $opengeodbinfo = rm_api_opengeodb_queryinfobyzip($zipcode);
    
    $_SESSION['regionselect'] = array(
        'zip' => $zipcode,
        'lat' => $opengeodbinfo['main_region']['lat'],
        'lon' => $opengeodbinfo['main_region']['lon'],
    );

    global $user;
    
    $queryparams = drupal_get_query_parameters();
    
    //Map Center
    if(empty($queryparams['map_center'])) {
        if(!isset($opengeodbinfo)) $opengeodbinfo = rm_api_opengeodb_queryinfobyzip($zipcode);
        drupal_add_js(array('rm_shop' => array('map_center' => $opengeodbinfo['main_region']['lat'] . ',' . $opengeodbinfo['main_region']['lon'])), 'setting');
    }
    else {
        drupal_add_js(array('rm_shop' => array('map_center' => $queryparams['map_center'])), 'setting');
    }
    
    //Map Bounds Southwest
    if(empty($queryparams['map_bounds_sw_lat']) || empty($queryparams['map_bounds_sw_lng'])) {
        $lat = $opengeodbinfo['main_region']['lat'] - 5;
        $lon = $opengeodbinfo['main_region']['lon'] - 5;
        if(!isset($opengeodbinfo)) $opengeodbinfo = rm_api_opengeodb_queryinfobyzip($zipcode);
        drupal_add_js(array('rm_shop' => array('map_bounds_sw_lat' => $lat)), 'setting');
        drupal_add_js(array('rm_shop' => array('map_bounds_sw_lng' => $lon)), 'setting');
    }
    else {
        drupal_add_js(array('rm_shop' => array('map_bounds_sw_lat' => $queryparams['map_bounds_sw_lat'])), 'setting');
        drupal_add_js(array('rm_shop' => array('map_bounds_sw_lng' => $queryparams['map_bounds_sw_lng'])), 'setting');
    }
    
    //Map Bounds Northeast
    if(empty($queryparams['map_bounds_ne_lat']) || empty($queryparams['map_bounds_ne_lng'])) {
        $lat = $opengeodbinfo['main_region']['lat'] + 5;
        $lon = $opengeodbinfo['main_region']['lon'] + 5;
        if(!isset($opengeodbinfo)) $opengeodbinfo = rm_api_opengeodb_queryinfobyzip($zipcode);
        drupal_add_js(array('rm_shop' => array('map_bounds_ne_lat' => $lat)), 'setting');
        drupal_add_js(array('rm_shop' => array('map_bounds_ne_lng' => $lon)), 'setting');
    }
    else {
        drupal_add_js(array('rm_shop' => array('map_bounds_ne_lat' => $queryparams['map_bounds_ne_lat'])), 'setting');
        drupal_add_js(array('rm_shop' => array('map_bounds_ne_lng' => $queryparams['map_bounds_ne_lng'])), 'setting');
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
    
    $opengeodbinfo = rm_api_opengeodb_queryinfobyzip($zipcode);
    $spread = 50;
    $rectangle['map_bounds_sw'][0] = $opengeodbinfo['main_region']['lat'] - $spread;
    $rectangle['map_bounds_sw'][1] = $opengeodbinfo['main_region']['lon'] - $spread;
    $rectangle['map_bounds_ne'][0] = $opengeodbinfo['main_region']['lat'] + $spread;
    $rectangle['map_bounds_ne'][1] = $opengeodbinfo['main_region']['lon'] + $spread;
    
    $shops = rm_shop_get_shop_agreements($user->uid, $zipcode, NULL, $rectangle, $queryparams);
    $alluidkeys = array();
    
    //Add marker to settings
    if(!empty($shops)) {
        foreach($shops as $shop) {
            $alluidkeys[] = $shop->uid;
            $address = '';
            $image_path = '';
            if( !empty($shop->field_address[LANGUAGE_NONE][0]['thoroughfare']) &&
                !empty($shop->field_address[LANGUAGE_NONE][0]['locality']) &&
                !empty($shop->field_location[LANGUAGE_NONE][0]['lat']) &&
                !empty($shop->field_location[LANGUAGE_NONE][0]['lon']) ) {
                $address = $shop->field_address[LANGUAGE_NONE][0]['thoroughfare'];
                if(isset($shop->field_address[LANGUAGE_NONE][0]['premise']) && !empty($shop->field_address[LANGUAGE_NONE][0]['premise'])) {
                    $address .= ', ' . $shop->field_address[LANGUAGE_NONE][0]['premise'];
                }
                if(!empty($shop->field_address[LANGUAGE_NONE][0]['postal_code'])) $address .= ', ' . $shop->field_address[LANGUAGE_NONE][0]['postal_code'];
                $address .= ' ' . $shop->field_address[LANGUAGE_NONE][0]['locality'];
            }
            
            if( !empty($shop->field_image[LANGUAGE_NONE][0]['uri']) ) {
                $image_path = image_style_url('mappopup_thumb', $shop->field_image[LANGUAGE_NONE][0]['uri']);
            }
            
            $markerarray[] = array(
                'name' => $shop->title,
                'url' => url('node/' . $shop->nid),
                'address' => $address,
                'lat' => $shop->field_location[LANGUAGE_NONE][0]['lat'],
                'lon' => $shop->field_location[LANGUAGE_NONE][0]['lon'],
                'type' => $shop->type,
                'marker_id' => $shop->nid,
                'image_path' => $image_path 
            );
        }
        drupal_add_js(array('rm_shop' => array('map_marker' => $markerarray)), 'setting');
    }
    $producttitlearray = array();
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'node');
    $query->entityCondition('bundle', array('offer_variation'));
    $query->propertyCondition('status', 1);
    $query->propertyCondition('uid', $alluidkeys);
    $result = $query->execute();
    $matchingproducts = array();
    if(isset($result['node'])) {
        $matchingproducts = entity_load('node', array_keys($result['node']));
    }
    if(!empty($matchingproducts)) {
        foreach($matchingproducts as $matchingproduct) {
            $producttitlearray[] = $matchingproduct->title;
        }
    }
    drupal_add_js(array('rm_shop' => array('products' => $producttitlearray)), 'setting');
    
    $categorytids = array();
    
    if(!empty($queryparams['map_bounds_sw_lng']) || !empty($queryparams['map_bounds_sw_lat']) || !empty($queryparams['map_bounds_ne_lng']) || !empty($queryparams['map_bounds_ne_lat']) || !empty($queryparams['seller_name']) || !empty($queryparams['seller_type']) || !empty($queryparams['delivery_options']) || !empty($queryparams['payment_types'])) {
        $catshops = rm_shop_get_shop_agreements($user->uid, $zipcode);
    }
    else {
        $catshops = $shops;
    }
    
    foreach($catshops as $catshop) {
        foreach($catshop->field_sellercategories[LANGUAGE_NONE] as $tids) {
            $categorytids[$tids['tid']] = TRUE;
        }
    }
    
    $categories = taxonomy_term_load_multiple(array_keys($categorytids));
    usort($categories, 'rm_shop_sort_seller_categories');

    //pass to tpl
    return theme('rm_shop_theme_shops', array(
        'vars' => array(
            'title' => t('Vendors in @region', array('@region' => arg(1))),
            'shops' => $shops,
            'pager' => theme('pager'),
            'categories' => $categories,
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

    $time = time();
    //Cache for 1 day = 86400 seconds
    $cacheexpire = $time - 86400;
    //Get children of parent item (cache this)
    $cached = cache_get('shopSitemapData');
    if($cached && $cached->created > $cacheexpire) {
        $groupednodes = $cached->data;
    }
    else {
        cache_clear_all('shopSitemapData', 'cache', TRUE);
        
        $result = db_select('node', 'n')
            ->fields('n', array('nid', 'title', 'type'))
            ->condition('status', 1)
            ->condition('type', array('simple_page', 'faq', 'customer_profile', 'job', 'seller_profile', 'trader_profile', 'media_object', 'press_report', 'labels'), 'IN')
            ->execute();
        
        $groupednodes = array();
        while($record = $result->fetchAssoc()) {
            $groupednodes[$record['type']][] = array(
                'title' => $record['title'],
                'nid' => $record['nid']
            );
        }
        cache_set('shopSitemapData', $groupednodes);
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