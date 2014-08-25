<?php

// function rm_seller_offermanagement() {
    // drupal_add_js(drupal_get_path('module', 'rm_seller') . '/js/rm_seller.js');
    // //pass to tpl
    // return theme('rm_seller_theme_offermanagement', array(
        // 'vars' => array(
            // 'form' => drupal_get_form('rm_seller_offer_form'),
            // 'offers' => rm_shop_get_structured_seller_offers(29, array(0,1)),
        // ),
    // ));
// }

function rm_seller_offer_form($form, &$form_state) {
    global $user;
    $uid = $user->uid;
    
    if(rm_user_is_admin()) $uid = 44;
    
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
            
            //Product origin
            $form['offers']['offer_' . $offer->nid]['these_fields']['field_origin-' . $offer->nid] = addressfield_generate($offer->field_origin[LANGUAGE_NONE][0], array('address' => 'address', 'organisation' => 'organisation'), array('mode' => 'form'));
            unset($form['offers']['offer_' . $offer->nid]['these_fields']['field_origin-' . $offer->nid]['street_block']);
            
            if(!empty($offer->field_origin[LANGUAGE_NONE][0]['organisation_name'])) $form['offers']['offer_' . $offer->nid]['these_fields']['field_origin-' . $offer->nid]['organisation_block']['organisation_name']['#value'] = $offer->field_origin[LANGUAGE_NONE][0]['organisation_name'];
            $form['offers']['offer_' . $offer->nid]['these_fields']['field_origin-' . $offer->nid]['organisation_block']['organisation_name']['#title_display'] = 'none';
            $form['offers']['offer_' . $offer->nid]['these_fields']['field_origin-' . $offer->nid]['organisation_block']['organisation_name']['#attributes']['placeholder'] = t('Organisation');
            $form['offers']['offer_' . $offer->nid]['these_fields']['field_origin-' . $offer->nid]['organisation_block']['organisation_name']['#attributes']['class'][] ='form-control';
            
            if(!empty($offer->field_origin[LANGUAGE_NONE][0]['country'])) $form['offers']['offer_' . $offer->nid]['these_fields']['field_origin-' . $offer->nid]['country']['#value'] = $offer->field_origin[LANGUAGE_NONE][0]['country'];
            $form['offers']['offer_' . $offer->nid]['these_fields']['field_origin-' . $offer->nid]['country']['#value'] = 'AT';
            $form['offers']['offer_' . $offer->nid]['these_fields']['field_origin-' . $offer->nid]['country']['#title_display'] = 'none';
            $form['offers']['offer_' . $offer->nid]['these_fields']['field_origin-' . $offer->nid]['country']['#attributes']['placeholder'] = t('Country');
            $form['offers']['offer_' . $offer->nid]['these_fields']['field_origin-' . $offer->nid]['country']['#attributes']['class'][] ='form-control';
            
            if(!empty($offer->field_origin[LANGUAGE_NONE][0]['locality'])) $form['offers']['offer_' . $offer->nid]['these_fields']['field_origin-' . $offer->nid]['locality_block']['locality']['#value'] = $offer->field_origin[LANGUAGE_NONE][0]['locality'];
            $form['offers']['offer_' . $offer->nid]['these_fields']['field_origin-' . $offer->nid]['locality_block']['locality']['#title_display'] = 'none';
            $form['offers']['offer_' . $offer->nid]['these_fields']['field_origin-' . $offer->nid]['locality_block']['locality']['#attributes']['placeholder'] = t('Locality');
            $form['offers']['offer_' . $offer->nid]['these_fields']['field_origin-' . $offer->nid]['locality_block']['locality']['#attributes']['class'][] ='form-control';
            
            if(!empty($offer->field_origin[LANGUAGE_NONE][0]['postal_code'])) $form['offers']['offer_' . $offer->nid]['these_fields']['field_origin-' . $offer->nid]['locality_block']['postal_code']['#value'] = $offer->field_origin[LANGUAGE_NONE][0]['postal_code'];
            $form['offers']['offer_' . $offer->nid]['these_fields']['field_origin-' . $offer->nid]['locality_block']['postal_code']['#title_display'] = 'none';
            $form['offers']['offer_' . $offer->nid]['these_fields']['field_origin-' . $offer->nid]['locality_block']['postal_code']['#attributes']['placeholder'] = t('Postal code');
            $form['offers']['offer_' . $offer->nid]['these_fields']['field_origin-' . $offer->nid]['locality_block']['postal_code']['#attributes']['class'][] ='form-control';
            
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
                // $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields']['field_image-' . $variation->nid] = array(
                    // '#type' => 'managed_file',
                    // '#title' => t('Images'),
                    // '#required' => TRUE,
                    // '#title_display' => 'none',
                    // '#attributes' => array(
                        // 'placeholder' => t('Images'),
                        // 'class' => array('form-control'),
                    // ),
                // );
                
                //Product unit
                $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields']['field_productunit_amount-' . $variation->nid] = array(
                    '#type' => 'textfield',
                    '#title' => t('Amount'),
                    '#required' => TRUE,
                    '#title_display' => 'none',
                    '#attributes' => array(
                        'placeholder' => t('Amount'),
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