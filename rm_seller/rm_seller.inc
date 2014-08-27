<?php

function rm_seller_dashboard() {

    //pass to tpl
    return theme('rm_seller_theme_dashboard', array(
        'vars' => array(
            'dashboard' => NULL,
        ),
    ));
}

function rm_seller_offer_form($form, &$form_state) {
    global $user;
    $uid = $user->uid;
    
    if(rm_user_is_admin()) $uid = 50;
    
    $offers = rm_shop_get_structured_seller_offers($uid, array(0,1));
    
    $form = array();
    
    
    if(isset($offers)) {
        foreach($offers as $offer) {
            $form['offers']['offer_' . $offer->nid]['these_fields'] = array();
            
            //Offer title
            $form['offers']['offer_' . $offer->nid]['these_fields']['field_description_title-' . $offer->nid] = array(
                '#type' => 'textfield',
                '#title' => t('Offer description'),
                '#required' => TRUE,
                '#title_display' => 'none',
                '#attributes' => array(
                    'placeholder' => t('Offer description'),
                    'class' => array('form-control'),
                    'required' => NULL,
                ),
            );
            if(!empty($offer->title)) $form['offers']['offer_' . $offer->nid]['these_fields']['field_description_title-' . $offer->nid]['#default_value'] = $offer->title;
            
            //Product origin company
            $form['offers']['offer_' . $offer->nid]['these_fields']['field_origin_company-' . $offer->nid] = array(
                '#type' => 'textfield',
                '#title' => t('Company'),
                '#title_display' => 'none',
                '#attributes' => array(
                    'placeholder' => t('Company'),
                    'class' => array('form-control'),
                ),
            );
            if(!empty($offer->field_origin[LANGUAGE_NONE][0]['organisation_name'])) $form['offers']['offer_' . $offer->nid]['these_fields']['field_origin_company-' . $offer->nid]['#default_value'] = $offer->field_origin[LANGUAGE_NONE][0]['organisation_name'];
            
            //Product origin zip
            $form['offers']['offer_' . $offer->nid]['these_fields']['field_origin_zip-' . $offer->nid] = array(
                '#type' => 'textfield',
                '#title' => t('Postal code'),
                '#title_display' => 'none',
                '#attributes' => array(
                    'placeholder' => t('Postal code'),
                    'class' => array('form-control'),
                ),
            );
            if(!empty($offer->field_origin[LANGUAGE_NONE][0]['postal_code'])) $form['offers']['offer_' . $offer->nid]['these_fields']['field_origin_zip-' . $offer->nid]['#default_value'] = $offer->field_origin[LANGUAGE_NONE][0]['postal_code'];
            
            //Product origin zip
            $form['offers']['offer_' . $offer->nid]['these_fields']['field_origin_locality-' . $offer->nid] = array(
                '#type' => 'textfield',
                '#title' => t('Locality'),
                '#title_display' => 'none',
                '#attributes' => array(
                    'placeholder' => t('Locality'),
                    'class' => array('form-control'),
                ),
            );
            if(!empty($offer->field_origin[LANGUAGE_NONE][0]['locality'])) $form['offers']['offer_' . $offer->nid]['these_fields']['field_origin_locality-' . $offer->nid]['#default_value'] = $offer->field_origin[LANGUAGE_NONE][0]['locality'];
            
            //Product origin country
            $form['offers']['offer_' . $offer->nid]['these_fields']['field_origin_country-' . $offer->nid] = array(
                '#type' => 'select',
                '#title' => t('Country'),
                '#options' => rm_api_get_iso_countries('de'),
                '#required' => TRUE,
                '#title_display' => 'none',
                '#attributes' => array(
                    'placeholder' => t('Country'),
                    'class' => array('form-control'),
                    'required' => NULL,
                ),
            );
            if(!empty($offer->field_origin[LANGUAGE_NONE][0]['country'])) $form['offers']['offer_' . $offer->nid]['these_fields']['field_origin_country-' . $offer->nid]['#default_value'] = $offer->field_origin[LANGUAGE_NONE][0]['country'];
            
            
            foreach($offer->offer_variations as $variation) {
                $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields'] = array();
                
                //Variation SKU
                $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields']['field_sku-' . $variation->nid] = array(
                    '#type' => 'textfield',
                    '#title' => t('SKU'),
                    '#required' => TRUE,
                    '#title_display' => 'none',
                    '#attributes' => array(
                        'placeholder' => t('SKU'),
                        'class' => array('form-control'),
                        'required' => NULL,
                    ),
                );
                if(!empty($variation->field_sku[LANGUAGE_NONE][0]['value'])) $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields']['field_sku-' . $variation->nid]['#default_value'] = $variation->field_sku[LANGUAGE_NONE][0]['value'];
                
                
                //Variation Title
                $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields']['field_variation_title-' . $variation->nid] = array(
                    '#type' => 'textfield',
                    '#title' => t('Variation title'),
                    '#required' => TRUE,
                    '#title_display' => 'none',
                    '#attributes' => array(
                        'placeholder' => t('Variation title'),
                        'class' => array('form-control'),
                        'required' => NULL,
                    ),
                );
                if(!empty($variation->title)) $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields']['field_variation_title-' . $variation->nid]['#default_value'] = $variation->title;
                
                //Body
                $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields']['body-' . $variation->nid] = array(
                    '#type' => 'textarea',
                    '#title' => t('Description'),
                    '#required' => TRUE,
                    '#title_display' => 'none',
                    '#attributes' => array(
                        'placeholder' => t('Description'),
                        'class' => array('form-control'),
                        'required' => NULL,
                    ),
                );
                if(!empty($variation->body[LANGUAGE_NONE][0]['value'])) $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields']['body-' . $variation->nid]['#default_value'] = strip_tags($variation->body[LANGUAGE_NONE][0]['value']);
                
                //Images
                $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields']['field_image-' . $variation->nid] = array(
                    '#type' => 'managed_file',
                    '#title' => t('Images'),
                    '#required' => FALSE,
                    '#title_display' => 'none',
                    '#upload_location' => 'public://product_images/',
                );
                
                //Product unit
                $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields']['field_productunit_amount-' . $variation->nid] = array(
                    '#type' => 'textfield',
                    '#title' => t('Quantity'),
                    '#required' => TRUE,
                    '#title_display' => 'none',
                    '#attributes' => array(
                        'placeholder' => t('Quantity'),
                        'class' => array('form-control'),
                        'required' => NULL,
                    ),
                );
                if(!empty($variation->field_productunit[LANGUAGE_NONE][0]['first'])) $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields']['field_productunit_amount-' . $variation->nid]['#default_value'] = $variation->field_productunit[LANGUAGE_NONE][0]['first'];
                
                $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields']['field_productunit_unit-' . $variation->nid] = array(
                    '#type' => 'select',
                    '#options' => array(
                        'pc' => t('pc'),
                        'bd' => t('bd'),
                        'kg' => t('kg'),
                        'g' => t('g'),
                        'l' => t('l'),
                        'ml' => t('ml'),
                        'pair' => t('pair'),
                    ),
                    '#title' => t('Unit'),
                    '#required' => TRUE,
                    '#title_display' => 'none',
                    '#attributes' => array(
                        'placeholder' => t('Unit'),
                        'class' => array('form-control'),
                        'required' => NULL,
                    ),
                );
                if(!empty($variation->field_productunit[LANGUAGE_NONE][0]['second'])) $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields']['field_productunit_unit-' . $variation->nid]['#default_value'] = $variation->field_productunit[LANGUAGE_NONE][0]['second'];
                
                //Stock
                $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields']['field_stock-' . $variation->nid] = array(
                    '#type' => 'textfield',
                    '#title' => t('Stock'),
                    '#required' => FALSE,
                    '#title_display' => 'none',
                    '#attributes' => array(
                        'placeholder' => t('Stock'),
                        'class' => array('form-control'),
                    ),
                );
                if(!empty($variation->field_stock[LANGUAGE_NONE][0]['value'])) $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields']['field_stock-' . $variation->nid]['#default_value'] = $variation->field_stock[LANGUAGE_NONE][0]['value'];
                
                //GTIN
                $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields']['field_gtin-' . $variation->nid] = array(
                    '#type' => 'textfield',
                    '#title' => t('GTIN'),
                    '#required' => FALSE,
                    '#title_display' => 'none',
                    '#attributes' => array(
                        'placeholder' => t('GTIN'),
                        'class' => array('form-control'),
                    ),
                );
                if(!empty($variation->field_gtin[LANGUAGE_NONE][0]['value'])) $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields']['field_gtin-' . $variation->nid]['#default_value'] = $variation->field_gtin[LANGUAGE_NONE][0]['value'];
                
                foreach($variation->trading_units as $tradingunit) {
                    $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['tradingunit_' . $tradingunit->nid]['these_fields'] = array();
                    
                    //TU Amount
                    $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['tradingunit_' . $tradingunit->nid]['these_fields']['field_tu_amount-' . $tradingunit->nid] = array(
                        '#type' => 'textfield',
                        '#title' => t('Quantity'),
                        '#required' => TRUE,
                        '#title_display' => 'none',
                        '#attributes' => array(
                            'placeholder' => t('Quantity'),
                            'class' => array('form-control'),
                            'required' => NULL,
                        ),
                    );
                    if(!empty($tradingunit->field_tu_amount[LANGUAGE_NONE][0]['value'])) $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['tradingunit_' . $tradingunit->nid]['these_fields']['field_tu_amount-' . $tradingunit->nid]['#default_value'] = $tradingunit->field_tu_amount[LANGUAGE_NONE][0]['value'];
                    
                    //TU Price
                    $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['tradingunit_' . $tradingunit->nid]['these_fields']['field_tu_price-' . $tradingunit->nid] = array(
                        '#type' => 'textfield',
                        '#title' => t('Price'),
                        '#required' => TRUE,
                        '#title_display' => 'none',
                        '#attributes' => array(
                            'placeholder' => t('Price'),
                            'class' => array('form-control'),
                            'required' => NULL,
                        ),
                    );
                    if(!empty($tradingunit->field_tu_price[LANGUAGE_NONE][0]['value'])) $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['tradingunit_' . $tradingunit->nid]['these_fields']['field_tu_price-' . $tradingunit->nid]['#default_value'] = $tradingunit->field_tu_price[LANGUAGE_NONE][0]['value'];
                    
                    //TU VAT
                    $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['tradingunit_' . $tradingunit->nid]['these_fields']['field_tu_vat-' . $tradingunit->nid] = array(
                        '#type' => 'select',
                        '#options' => array(
                            '0' => '0%',
                            '7' => '7%',
                            '10.7' => '10.7%',
                            '19' => '19%',
                        ),
                        '#title' => t('VAT'),
                        '#required' => TRUE,
                        '#title_display' => 'none',
                        '#attributes' => array(
                            'placeholder' => t('VAT'),
                            'class' => array('form-control'),
                            'required' => NULL,
                        ),
                    );
                    if(!empty($tradingunit->field_tu_vat[LANGUAGE_NONE][0]['value'])) $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['tradingunit_' . $tradingunit->nid]['these_fields']['field_tu_vat-' . $tradingunit->nid]['#default_value'] = $tradingunit->field_tu_vat[LANGUAGE_NONE][0]['value'];
                    
                    //TU Deposit
                    $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['tradingunit_' . $tradingunit->nid]['these_fields']['field_tu_deposit-' . $tradingunit->nid] = array(
                        '#type' => 'textfield',
                        '#title' => t('Deposit'),
                        '#required' => TRUE,
                        '#title_display' => 'none',
                        '#attributes' => array(
                            'placeholder' => t('Deposit'),
                            'class' => array('form-control'),
                            'required' => NULL,
                        ),
                    );
                    if(!empty($tradingunit->field_tu_deposit[LANGUAGE_NONE][0]['value'])) $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['tradingunit_' . $tradingunit->nid]['these_fields']['field_tu_deposit-' . $tradingunit->nid]['#default_value'] = $tradingunit->field_tu_deposit[LANGUAGE_NONE][0]['value'];
                    
                    //TU Packaging
                    $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['tradingunit_' . $tradingunit->nid]['these_fields']['field_tu_packaging-' . $tradingunit->nid] = array(
                        '#type' => 'select',
                        '#options' => array(
                            'box' => t('Box'),
                            'crate' => t('Crate'),
                            'bundle' => t('Bundle'),
                            'pallet' => t('Pallet'),
                            'vacuum' => t('Vacuum package'),
                            'single' => t('Single item'),
                            'bouquet' => t('Bouquet'),
                            'basket' => t('Basket'),
                        ),
                        '#title' => t('Trading unit type'),
                        '#required' => TRUE,
                        '#title_display' => 'none',
                        '#attributes' => array(
                            'placeholder' => t('Trading unit type'),
                            'class' => array('form-control'),
                            'required' => NULL,
                        ),
                    );
                    if(!empty($tradingunit->field_tu_packaging[LANGUAGE_NONE][0]['value'])) $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['tradingunit_' . $tradingunit->nid]['these_fields']['field_tu_packaging-' . $tradingunit->nid]['#default_value'] = $tradingunit->field_tu_packaging[LANGUAGE_NONE][0]['value'];
                }
            
            }
            
        }
    }
    $form['offers']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save changes'),
		'#suffix' => ' ' . l(t('Add another offer'), 'addofferdescription/' . $uid, array('query' => drupal_get_destination(), 'html' => TRUE, 'attributes' => array('class' => array('btn', 'btn-info', 'btn-lg')))),
    );
    return $form;
}

function rm_seller_offer_form_submit($form, &$form_state) {
    if(!empty($form_state['values'])) {
        foreach($form_state['values'] as $field_name => $field_value) {
            if($field_value != '') {
                $tmp = explode('-', $field_name);
                if(!empty($tmp[1]) && is_numeric($tmp[1])) {
                    $nodeobject = node_load($tmp[1]);
                    switch($tmp[0]) {
                        //Title fields
                        case 'field_description_title':
                        case 'field_variation_title':
                            $nodeobject->title = $field_value;
                            break;
                            
                        //Value fields with identical names
                        case 'field_sku':
                        case 'field_stock':
                        case 'field_gtin':
                        case 'field_tu_amount':
                        case 'field_tu_price':
                        case 'field_tu_vat':
                        case 'field_tu_deposit':
                        case 'field_tu_packaging':
                        case 'field_tu_vat':
                        case 'body':
                            $nodeobject->{$tmp[0]}[LANGUAGE_NONE][0]['value'] = $field_value;
                            break;
                        
                        //Origin fields
                        case 'field_origin_company':
                            $nodeobject->field_origin[LANGUAGE_NONE][0]['organisation_name'] = $field_value;
                            break;
                        case 'field_origin_zip':
                            $nodeobject->field_origin[LANGUAGE_NONE][0]['postal_code'] = $field_value;
                            break;
                        case 'field_origin_locality':
                            $nodeobject->field_origin[LANGUAGE_NONE][0]['locality'] = $field_value;
                            break;
                        case 'field_origin_country':
                            $nodeobject->field_origin[LANGUAGE_NONE][0]['country'] = $field_value;
                            break;
                        
                        //Product unit
                        case 'field_productunit_amount':
                            $nodeobject->field_productunit[LANGUAGE_NONE][0]['first'] = $field_value;
                            break;
                        case 'field_productunit_unit':
                            $nodeobject->field_productunit[LANGUAGE_NONE][0]['second'] = $field_value;
                            break;
                        
                        //Image
                        case 'field_image':
                            $file = file_load($field_value);
                            $file->status = FILE_STATUS_PERMANENT;
                            file_save($file);
                            $filearray = (array) $file;
                            $nodeobject->{$tmp[0]}[LANGUAGE_NONE][] = (array) $file;
                            break;
                            
                    }
                    node_save($nodeobject);
                }
            }
        }
    }
    drupal_set_message(t('All offers have been updated with the given values'), 'status');
}

function rm_seller_add_tradingunit($nid) {
    $variation = node_load($nid);
	$new_trading_unit = rm_api_create_new_node('trading_unit', node_type_get_name('trading_unit'), 'de', $variation->uid, 0, 0, 0, 0);
    $variation->field_trading_unit_reference[LANGUAGE_NONE][]['target_id'] = $new_trading_unit->nid;
    node_save($variation);
    drupal_goto();
}

function rm_seller_add_offervariation($nid) {
    $offerdescription = node_load($nid);
	$new_trading_unit = rm_api_create_new_node('trading_unit', node_type_get_name('trading_unit'), 'de', $offerdescription->uid, 0, 0, 0, 0);
	$new_offer_variation = rm_api_create_new_node('offer_variation', node_type_get_name('offer_variation'), 'de', $offerdescription->uid, 0, 0, 0, 0);
    $new_offer_variation->field_trading_unit_reference[LANGUAGE_NONE][]['target_id'] = $new_trading_unit->nid;
    node_save($new_offer_variation);
    $offerdescription->field_offer_variation_reference[LANGUAGE_NONE][]['target_id'] = $new_offer_variation->nid;
    node_save($offerdescription);
    drupal_goto();
}

function rm_seller_add_offerdescription($uid) {
	$new_trading_unit = rm_api_create_new_node('trading_unit', node_type_get_name('trading_unit'), 'de', $uid, 0, 0, 0, 0);
	$new_offer_variation = rm_api_create_new_node('offer_variation', node_type_get_name('offer_variation'), 'de', $uid, 0, 0, 0, 0);
	$new_offer_description = rm_api_create_new_node('offer_description', node_type_get_name('offer_description'), 'de', $uid, 0, 0, 0, 0);
    $new_offer_variation->field_trading_unit_reference[LANGUAGE_NONE][]['target_id'] = $new_trading_unit->nid;
    node_save($new_offer_variation);
    $new_offer_description->field_offer_variation_reference[LANGUAGE_NONE][]['target_id'] = $new_offer_variation->nid;
    node_save($new_offer_description);
    drupal_goto();
}

function rm_seller_editprofile($form, &$form_state) {
    $form = array();
    return $form;
}

function rm_seller_pause_offer($nid) {
    $nodeobject = node_load($nid);
    $nodeobject->status = 0;
    node_save($nodeobject);
    if(!rm_api_is_ajax()) {
        drupal_set_message(t('Offer is now paused'), 'status');
        drupal_goto();
    }
}

function rm_seller_activate_offer($nid) {
    $nodeobject = node_load($nid);
    $nodeobject->status = 1;
    node_save($nodeobject);
    if(!rm_api_is_ajax()) {
        drupal_set_message(t('Offer is now active'), 'status');
        drupal_goto();
    }
}

function rm_seller_delete_offer($form, &$form_state, $nid) {
	
	$nodeobject = node_load($nid);
	
	$form['rm_deletenode']['nid'] = array(
		'#type' => 'value',
		'#value' => $nid,
	);
	
    $form['rm_deletenode']['type'] = array(
		'#type' => 'value',
		'#value' => $nodeobject->type,
	);
    
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Delete'),
		'#prefix' => '<p>' . t('Do you really want to delete the offer "@entry"?', array('@entry' => $nodeobject->title)) . '</p>',
		'#suffix' => l(t('Cancel'), $_GET['destination']),
	);
	
	return $form;
}

function rm_seller_delete_trading_unit($nid) {
    //Find out to which offer variation this trading unit is linked
    $query = new EntityFieldQuery();
    $tmp = $query
        ->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', array('offer_variation'))
        ->fieldCondition('field_trading_unit_reference', 'target_id', $nid)
        ->execute();
    if(isset($tmp['node'])) {
        //Remove link from offer variations
        $keys = array_keys($tmp['node']);
        $allvariations = entity_load('node', $keys);
        foreach($allvariations as $variation) {
            foreach($variation->field_trading_unit_reference[LANGUAGE_NONE] as $delta => $target_id) {
                if($target_id['target_id'] == $nid) {
                    unset($variation->field_trading_unit_reference[LANGUAGE_NONE][$delta]);
                }
            }
            node_save($variation);
            watchdog('rm_seller', t('Unlinked trading unit @nid from offer variation @vnid', array('@nid' => $nid, '@vnid' => $variation->nid)));
        }
    }
    //Delete trading unit
    node_delete($nid);
    watchdog('rm_seller', t('Deleted trading unit @nid', array('@nid' => $nid)));
}

function rm_seller_delete_offer_variation($nid) {
    //Find out to which offer descriptions this offer variation is linked
    $query = new EntityFieldQuery();
    $tmp = $query
        ->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', array('offer_description'))
        ->fieldCondition('field_offer_variation_reference', 'target_id', $nid)
        ->execute();
    if(isset($tmp['node'])) {
        //Remove link from offer descriptions
        $keys = array_keys($tmp['node']);
        $alldescriptions = entity_load('node', $keys);
        foreach($alldescriptions as $description) {
            foreach($description->field_offer_variation_reference[LANGUAGE_NONE] as $delta => $target_id) {
                if($target_id['target_id'] == $nid) {
                    unset($description->field_offer_variation_reference[LANGUAGE_NONE][$delta]);
                }
            }
            node_save($description);
            watchdog('rm_seller', t('Unlinked offer variation @nid from offer description @dnid', array('@nid' => $nid, '@vnid' => $description->nid)));
        }
    }
    //Delete all trading units from offer_variation
    $offer_variation = node_load($nid);
    if(isset($offer_variation->field_trading_unit_reference[LANGUAGE_NONE])) {
        foreach($offer_variation->field_trading_unit_reference[LANGUAGE_NONE] as $delta => $target) {
            rm_seller_delete_trading_unit($target['target_id']);
        }
    }
    //Delete offer variation
    node_delete($nid);
    watchdog('rm_seller', t('Deleted offer variation @nid', array('@nid' => $nid)));
}

function rm_seller_delete_offer_description($nid) {
    //Delete all offer variations offer_description
    $offer_description = node_load($nid);
    if(isset($offer_description->field_offer_variation_reference[LANGUAGE_NONE])) {
        foreach($offer_description->field_offer_variation_reference[LANGUAGE_NONE] as $vdelta => $vtarget) {
            $offer_variation = node_load($vtarget['target_id']);
            if(isset($offer_variation->field_trading_unit_reference[LANGUAGE_NONE])) {
                foreach($offer_variation->field_trading_unit_reference[LANGUAGE_NONE] as $delta => $target) {
                    rm_seller_delete_trading_unit($target['target_id']);
                }
            }            
            rm_seller_delete_offer_variation($vtarget['target_id']);
        }
    }
    //Delete offer description
    node_delete($nid);
    watchdog('rm_seller', t('Deleted offer description @nid', array('@nid' => $nid)));
}

function rm_seller_delete_offer_submit($form, &$form_state) {
	$title = node_load($form_state['values']['nid'])->title;
    global $user;
    switch($form_state['values']['type']) {
        case 'trading_unit':
            rm_seller_delete_trading_unit($form_state['values']['nid']);
            drupal_set_message(t('Deleted the trading unit @entry', array('@entry' => $title)), 'status');
            rm_user_activity(t('Deleted the trading unit @entry', array('@entry' => $title)), NULL, NULL, $user->uid);
            break;
        case 'offer_variation':
            rm_seller_delete_offer_variation($form_state['values']['nid']);
            drupal_set_message(t('Deleted the offer variation @entry', array('@entry' => $title)), 'status');
            rm_user_activity(t('Deleted the offer variation @entry', array('@entry' => $title)), NULL, NULL, $user->uid);
            break;
        case 'offer_description':
            rm_seller_delete_offer_description($form_state['values']['nid']);
            drupal_set_message(t('Deleted the offer description @entry', array('@entry' => $title)), 'status');
            rm_user_activity(t('Deleted the offer description @entry', array('@entry' => $title)), NULL, NULL, $user->uid);
            break;
    }
	
}

function rm_seller_delete_image($nid, $delta) {
    rm_api_delete_file_from_node($nid, 'field_image', $delta);
    drupal_goto();
}