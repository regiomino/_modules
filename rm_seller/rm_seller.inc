<?php

function rm_seller_offermanagement() {
    drupal_add_js(drupal_get_path('module', 'rm_seller') . '/js/rm_seller.js');
    //pass to tpl
    return theme('rm_seller_theme_offermanagement', array(
        'vars' => array(
            'form' => drupal_get_form('rm_seller_offer_form'),
            'offers' => rm_shop_get_structured_seller_offers(29, array(0,1)),
        ),
    ));
}

function rm_seller_offer_form($form, &$form_state) {
    global $user;
    $uid = $user->uid;
    
    if(rm_user_is_admin()) $uid = 29;
    
    $offers = rm_shop_get_structured_seller_offers($uid, array(0,1));
    
    $form = array();
    
    if(isset($offers)) {
        foreach($offers as $offer) {
            $form['offers']['offer_' . $offer->nid]['these_fields'] = array();
            rm_api_attach_field_to_form('field_origin', 'node', 'offer_description', $offer, $form['offers']['offer_' . $offer->nid]['these_fields'], $form_state, LANGUAGE_NONE);
            rm_api_attach_field_to_form('field_label_reference', 'node', 'offer_description', $offer, $form['offers']['offer_' . $offer->nid]['these_fields'], $form_state, LANGUAGE_NONE);
            unset($form['offers']['offer_' . $offer->nid]['these_fields']['field_origin'][LANGUAGE_NONE][0]['street_block']);
            foreach($offer->offer_variations as $variation) {
                $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields'] = array();
                rm_api_attach_field_to_form('field_sku', 'node', 'offer_variation', $variation, $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields'], $form_state, LANGUAGE_NONE);
                $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields']['field_title'] = array(
                    '#type' => 'textfield',
                    '#title' => t('Variation description'),
                    '#required' => TRUE,
                );
                rm_api_attach_field_to_form('body', 'node', 'offer_variation', $variation, $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields'], $form_state, LANGUAGE_NONE);
                rm_api_attach_field_to_form('field_productunit', 'node', 'offer_variation', $variation, $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields'], $form_state, LANGUAGE_NONE);
                rm_api_attach_field_to_form('field_image', 'node', 'offer_variation', $variation, $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields'], $form_state, LANGUAGE_NONE);
                rm_api_attach_field_to_form('field_stock', 'node', 'offer_variation', $variation, $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields'], $form_state, LANGUAGE_NONE);
                rm_api_attach_field_to_form('field_gtin', 'node', 'offer_variation', $variation, $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields'], $form_state, LANGUAGE_NONE);
                foreach($variation->trading_units as $tradingunit) {
                    $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['tradingunit_' . $tradingunit->nid]['these_fields'] = array();
                    rm_api_attach_field_to_form('field_tu_amount', 'node', 'trading_unit', $tradingunit, $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['tradingunit_' . $tradingunit->nid]['these_fields'], $form_state, LANGUAGE_NONE);
                    rm_api_attach_field_to_form('field_tu_price', 'node', 'trading_unit', $tradingunit, $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['tradingunit_' . $tradingunit->nid]['these_fields'], $form_state, LANGUAGE_NONE);
                    rm_api_attach_field_to_form('field_tu_deposit', 'node', 'trading_unit', $tradingunit, $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['tradingunit_' . $tradingunit->nid]['these_fields'], $form_state, LANGUAGE_NONE);
                    rm_api_attach_field_to_form('field_tu_vat', 'node', 'trading_unit', $tradingunit, $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['tradingunit_' . $tradingunit->nid]['these_fields'], $form_state, LANGUAGE_NONE);
                    rm_api_attach_field_to_form('field_tu_packaging', 'node', 'trading_unit', $tradingunit, $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['tradingunit_' . $tradingunit->nid]['these_fields'], $form_state, LANGUAGE_NONE);
                }
            }
        }
    }
    $form['offers']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save changes'),
		'#suffix' => ' ' . l(t('Add another offer'), 'addofferdescription/' . $uid, array('html' => TRUE, 'attributes' => array('class' => array('btn', 'btn-info', 'btn-lg')))),
    );
    return $form;
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
}

function rm_seller_add_offerdescription($uid) {
	$new_trading_unit = rm_api_create_new_node('trading_unit', node_type_get_name('trading_unit'), 'de', $offerdescription->uid, 0, 0, 0, 0);
	$new_offer_variation = rm_api_create_new_node('offer_variation', node_type_get_name('offer_variation'), 'de', $offerdescription->uid, 0, 0, 0, 0);
	$new_offer_description = rm_api_create_new_node('offer_description', node_type_get_name('offer_description'), 'de', $offerdescription->uid, 0, 0, 0, 0);
    $new_offer_variation->field_trading_unit_reference[LANGUAGE_NONE][]['target_id'] = $new_trading_unit->nid;
    node_save($new_offer_variation);
    $new_offer_description->field_offer_variation_reference[LANGUAGE_NONE][]['target_id'] = $new_offer_variation->nid;
    node_save($new_offer_description);
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
    $query = new EntityFieldQuery();
    $tmp = $query
        ->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', array('offer_variation'))
        ->fieldCondition('field_trading_unit_reference', 'target_id', $nid)
        ->execute();
    if(isset($tmp['node'])) {
        $keys = array_keys($tmp['node']);
        $allvariations = entity_load('node', $keys);
        foreach($allvariations as $variation) {
            foreach($variation->field_trading_unit_reference[LANGUAGE_NONE] as $delta => $target_id) {
                if($target_id['target_id'] == $nid) {
                    unset($variation->field_trading_unit_reference[LANGUAGE_NONE][$delta]);
                }
            }
            node_save($variation);
        }
    }
    node_delete($nid);
}

function rm_seller_delete_offer_variation($nid) {

}

function rm_seller_delete_offer_description($nid) {

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
            drupal_set_message(t('Deleted the trading unit @entry', array('@entry' => $title)), 'status');
            rm_user_activity(t('Deleted the trading unit @entry', array('@entry' => $title)), NULL, NULL, $user->uid);
            break;
        case 'offer_description':
            rm_seller_delete_offer_description($form_state['values']['nid']);
            drupal_set_message(t('Deleted the trading unit @entry', array('@entry' => $title)), 'status');
            rm_user_activity(t('Deleted the trading unit @entry', array('@entry' => $title)), NULL, NULL, $user->uid);
            break;
    }
	
}