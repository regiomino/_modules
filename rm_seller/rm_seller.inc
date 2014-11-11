<?php

function rm_seller_orders() {
    global $user;
    $orders = rm_cart_get_orders_grouped_by_order_number($user->uid);
    return theme('rm_seller_theme_orders', array(
        'vars' => array(
            'orders' => $orders,
        ),
    ));
}

function rm_seller_agreement_delete($form, &$form_state, $nid) {
	
	$nodeobject = node_load($nid);
	
	$form['rm_deleteagreement']['nid'] = array(
		'#type' => 'value',
		'#value' => $nid,
	);
	
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Delete'),
		'#prefix' => '<p>' . t('Do you really want to delete the entry "@entry"?', array('@entry' => $nodeobject->title)) . '</p>',
		'#suffix' => l(t('Cancel'), $_GET['destination']),
	);
	
	return $form;
}

function rm_seller_agreement_delete_submit($form, &$form_state) {
	$title = node_load($form_state['values']['nid'])->title;
	node_delete($form_state['values']['nid']);
	global $user;
	drupal_set_message(t('Deleted the node @entry', array('@entry' => $title)), 'status');
	rm_user_activity(t('Deleted the node @entry', array('@entry' => $title)), NULL, NULL, $user->uid);
}

function rm_seller_agreement_add($form, &$form_state, $agreement_type, $suid) {

	$instances = field_info_instances('node', $agreement_type);

	$form['rm_add_agreement'] = array();
	
	foreach($instances as $field_name => $field_description) {
		rm_api_attach_field_to_form($field_name, 'node', $agreement_type, NULL, $form['rm_add_agreement'], $form_state, LANGUAGE_NONE);
        if($field_name == 'field_user_reference') {
            $form['rm_add_agreement']['field_user_reference'][LANGUAGE_NONE]['#options'][0] = t('for all');
            ksort($form['rm_add_agreement']['field_user_reference'][LANGUAGE_NONE]['#options']);
        }
        else if($field_name == 'field_location') {
            unset($instances[$field_name]);
        }
	}
	
	$form['rm_add_agreement']['agreement_type'] = array(
		'#type' => 'value',
		'#value' => $agreement_type,
	);
	
	$form['rm_add_agreement']['field_names'] = array(
		'#type' => 'value',
		'#value' => $instances,
	);
	
	$form['rm_add_agreement']['suid'] = array(
		'#type' => 'value',
		'#value' => $suid,
	);
	
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save'),
	);
	
	return $form;
}

function rm_seller_agreement_add_validate($form, &$form_state) {
}

function rm_seller_agreement_add_submit($form, &$form_state) {
    $title = node_type_get_name($form_state['values']['agreement_type']);
    $new_node = rm_api_create_new_node($form_state['values']['agreement_type'], $title, 'de', $form_state['values']['suid'], 1, 0, 0, 0);
    
    foreach($form_state['values']['field_names'] as $field_name => $field_definitions) {
        if($field_name == 'field_regular_times') {
            foreach($form_state['values'][$field_name][LANGUAGE_NONE] as $index => $dayvalues) {
                if($dayvalues['starthours'] == '' || $dayvalues['endhours'] = '') unset($form_state['values'][$field_name][LANGUAGE_NONE][$index]);
                if(!is_numeric($index)) unset($form_state['values'][$field_name][LANGUAGE_NONE][$index]);
            }
        }
        else if(count($form_state['values'][$field_name][LANGUAGE_NONE]) > 1) {
            foreach($form_state['values'][$field_name][LANGUAGE_NONE] as $index => $values) {
                if(!is_array($values) || empty($values['value'])) unset($form_state['values'][$field_name][LANGUAGE_NONE][$index]);
            }
        }
        $form_state['values'][$field_name][LANGUAGE_NONE] = array_values($form_state['values'][$field_name][LANGUAGE_NONE]);
        $new_node->{$field_name} = $form_state['values'][$field_name];
    }
    node_save($new_node);
}


function rm_seller_agreement_edit($form, &$form_state, $agreement_id) {
	
	$agreement = node_load($agreement_id);
	
	$instances = field_info_instances('node', $agreement->type);

	$form['rm_edit_agreement'] = array();
	
	foreach($instances as $field_name => $field_description) {
		rm_api_attach_field_to_form($field_name, 'node', $agreement->type, $agreement, $form['rm_edit_agreement'], $form_state, LANGUAGE_NONE);
        if($field_name == 'field_user_reference') {
            $form['rm_edit_agreement']['field_user_reference'][LANGUAGE_NONE]['#options'][0] = t('for all');
            ksort($form['rm_edit_agreement']['field_user_reference'][LANGUAGE_NONE]['#options']);
        }
        else if($field_name == 'field_location') {
            unset($instances[$field_name]);
        }
	}
	
	$form['rm_edit_agreement']['field_names'] = array(
		'#type' => 'value',
		'#value' => $instances,
	);
	
	$form['rm_edit_agreement']['nid'] = array(
		'#type' => 'value',
		'#value' => $agreement_id,
	);
	
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save'),
        '#suffix' => l(t('Delete'), 'manage/seller/deleteagreement/' . $agreement_id, array('query' => array('destination' => $_GET['destination']))),
	);
	
	return $form;
}

function rm_seller_agreement_edit_validate($form, &$form_state) {
}

function rm_seller_agreement_edit_submit($form, &$form_state) {
	$nodeobject = node_load($form_state['values']['nid']);

	foreach($form_state['values']['field_names'] as $field_name => $field_definitions) {
        if($field_name == 'field_regular_times') {
            foreach($form_state['values'][$field_name][LANGUAGE_NONE] as $index => $dayvalues) {
                if($dayvalues['starthours'] == '' || $dayvalues['endhours'] = '') unset($form_state['values'][$field_name][LANGUAGE_NONE][$index]);
                if(!is_numeric($index)) unset($form_state['values'][$field_name][LANGUAGE_NONE][$index]);
            }
        }
        else if(count($form_state['values'][$field_name][LANGUAGE_NONE]) > 1) {
            foreach($form_state['values'][$field_name][LANGUAGE_NONE] as $index => $values) {
                if(!is_array($values) || $values['value'] == '' || is_null($values['value'])) unset($form_state['values'][$field_name][LANGUAGE_NONE][$index]);
            }
        }
		$form_state['values'][$field_name][LANGUAGE_NONE] = array_values($form_state['values'][$field_name][LANGUAGE_NONE]);
		$nodeobject->{$field_name} = $form_state['values'][$field_name];
	}
	node_save($nodeobject);
}

function rm_seller_paymentagreements() {
    global $user;
    //pass to tpl
    return theme('rm_seller_theme_paymentagreements', array(
        'vars' => array(
            'agreements' => rm_api_get_nodes_by_properties(array('payment_agreement'), 1, -1, -1, -1, -1, $user->uid),
            'paymenttypes' => list_allowed_values(field_info_field('field_payment_types')),
        ),
    ));
}

function rm_seller_shippingagreements() {
    global $user;
    //pass to tpl
    return theme('rm_seller_theme_shippingagreements', array(
        'vars' => array(
            'agreements' => rm_api_get_nodes_by_properties(array('shipping_agreement'), 1, -1, -1, -1, -1, $user->uid),
        ),
    ));

}

function rm_seller_pickupagreements() {
    global $user;
    //pass to tpl
    return theme('rm_seller_theme_pickupagreements', array(
        'vars' => array(
            'agreements' => rm_api_get_nodes_by_properties(array('pickup_agreement'), 1, -1, -1, -1, -1, $user->uid),
        ),
    ));
}

function rm_seller_commissionagreements() {
    global $user;
    //pass to tpl
    return theme('rm_seller_theme_commissionagreements', array(
        'vars' => array(
            'agreements' => rm_api_get_nodes_by_properties(array('commission_agreement'), 1, -1, -1, -1, -1, $user->uid),
        ),
    ));
}

function rm_seller_dashboard() {
    global $user;
    $offers = rm_shop_get_structured_seller_offers($user->uid, array(0,1));
    $profiles = rm_api_get_nodes_by_properties(array('seller_profile'), 1, -1, -1, -1, -1, $user->uid);
    $profileskeys = array_keys($profiles);
    $profile = $profiles[$profileskeys[0]];
    $sortedorders = rm_cart_get_orders_grouped_by_order_number($user->uid, array('processing', 'accepted', 'delivered'));
    //pass to tpl
    return theme('rm_seller_theme_dashboard', array(
        'vars' => array(
            'offers' => $offers,
            'offercount' => count($offers),
            'profile' => $profile,
            'profilecompleteness' => rm_user_get_profile_completeness($profile->nid, 'seller_profile'),
            'accountcompleteness' => rm_user_get_account_completeness($user->uid),
            'paymentagreements' => rm_api_get_nodes_by_properties(array('payment_agreement'), 1, -1, -1, -1, -1, $user->uid),
            'shippingagreements' => rm_api_get_nodes_by_properties(array('shipping_agreement'), 1, -1, -1, -1, -1, $user->uid),
            'pickupagreements' => rm_api_get_nodes_by_properties(array('pickup_agreement'), 1, -1, -1, -1, -1, $user->uid),
            'orders' => $sortedorders,
            'ordercount' => count($sortedorders),
        ),
    ));
}

function rm_seller_offer_form($form, &$form_state) {
    global $user;
    $uid = $user->uid;
    $seller_user_object = user_load($uid);
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
                '#default_value' => $seller_user_object->field_address[LANGUAGE_NONE][0]['postal_code'],
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
                '#default_value' => $seller_user_object->field_address[LANGUAGE_NONE][0]['locality'],
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
                    
                ),
                '#default_value' => $seller_user_object->field_address[LANGUAGE_NONE][0]['country'],
            );
            if(!empty($offer->field_origin[LANGUAGE_NONE][0]['country'])) $form['offers']['offer_' . $offer->nid]['these_fields']['field_origin_country-' . $offer->nid]['#default_value'] = $offer->field_origin[LANGUAGE_NONE][0]['country'];
            
            $form['offers']['offer_' . $offer->nid]['these_fields']['add_variation_button-' . $offer->nid] = array(
                '#type' => 'button',
                '#value' => t('Add offer variation'),
                '#name' => 'add_variation_to-' . $offer->nid,
                '#executes_submit_callback' => TRUE,
            );
            $form['offers']['offer_' . $offer->nid]['these_fields']['add_variation_button-' . $offer->nid]['#attributes']['class'][] = 'btn-success';
            $form['offers']['offer_' . $offer->nid]['these_fields']['add_variation_button-' . $offer->nid]['#attributes']['class'][] = 'btn';
            
            
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
                
                //Lead time
                $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields']['field_leadtime-' . $variation->nid] = array(
                    '#type' => 'textfield',
                    '#title' => t('Lead time'),
                    '#required' => FALSE,
                    '#title_display' => 'none',
                    '#attributes' => array(
                        'placeholder' => t('Lead time'),
                        'class' => array('form-control'),
                    ),
                );
                if(!empty($variation->field_leadtime[LANGUAGE_NONE][0]['value'])) $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields']['field_leadtime-' . $variation->nid]['#default_value'] = $variation->field_leadtime[LANGUAGE_NONE][0]['value'];
                
                //Use within
                $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields']['field_use_within-' . $variation->nid] = array(
                    '#type' => 'textfield',
                    '#title' => t('Use within'),
                    '#required' => FALSE,
                    '#title_display' => 'none',
                    '#attributes' => array(
                        'placeholder' => t('Use within'),
                        'class' => array('form-control'),
                    ),
                );
                if(!empty($variation->field_use_within[LANGUAGE_NONE][0]['value'])) $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields']['field_use_within-' . $variation->nid]['#default_value'] = $variation->field_use_within[LANGUAGE_NONE][0]['value'];
                
                //Expiration date
                $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields']['field_expiration_date-' . $variation->nid] = array(
                    '#type' => 'date',
                    '#title' => t('Expiration date'),
                    '#required' => FALSE,
                    '#title_display' => 'none',
                    // '#pre_render' => array('rm_seller_pre_render_date_empty'),
                    // '#element_validate' => array('rm_seller_validate_date_empty'),
                );
                if(!empty($variation->field_expiration_date[LANGUAGE_NONE][0]['value'])) {
                    $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields']['field_expiration_date-' . $variation->nid]['#default_value']['day'] = date('j', $variation->field_expiration_date[LANGUAGE_NONE][0]['value']);
                    $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields']['field_expiration_date-' . $variation->nid]['#default_value']['month'] = date('n', $variation->field_expiration_date[LANGUAGE_NONE][0]['value']);
                    $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields']['field_expiration_date-' . $variation->nid]['#default_value']['year'] = date('Y', $variation->field_expiration_date[LANGUAGE_NONE][0]['value']);
                }
                
                $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields']['add_tu_button-' . $variation->nid] = array(
                    '#type' => 'button',
                    '#value' => t('Add trading unit'),
                    '#name' => 'add_tu_to-' . $variation->nid,
                    '#executes_submit_callback' => TRUE,
                );
                $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields']['add_tu_button-' . $variation->nid]['#attributes']['class'][] = 'btn-success';
                $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['these_fields']['add_tu_button-' . $variation->nid]['#attributes']['class'][] = 'btn';
                
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
                            
                        ),
                    );
                    if(!empty($tradingunit->field_tu_price[LANGUAGE_NONE][0]['value'])) $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['tradingunit_' . $tradingunit->nid]['these_fields']['field_tu_price-' . $tradingunit->nid]['#default_value'] = str_replace('.', ',', $tradingunit->field_tu_price[LANGUAGE_NONE][0]['value']);
                    
                    //TU RRP
                    $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['tradingunit_' . $tradingunit->nid]['these_fields']['field_tu_rrp-' . $tradingunit->nid] = array(
                        '#type' => 'textfield',
                        '#title' => t('RRP'),
                        '#title_display' => 'none',
                        '#attributes' => array(
                            'placeholder' => t('RRP'),
                            'class' => array('form-control'),
                        ),
                    );
                    if(!empty($tradingunit->field_tu_rrp[LANGUAGE_NONE][0]['value'])) $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['tradingunit_' . $tradingunit->nid]['these_fields']['field_tu_rrp-' . $tradingunit->nid]['#default_value'] = str_replace('.', ',', $tradingunit->field_tu_rrp[LANGUAGE_NONE][0]['value']);
                    
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
                            
                        ),
                    );
                    if(!empty($tradingunit->field_tu_deposit[LANGUAGE_NONE][0]['value'])) $form['offers']['offer_' . $offer->nid]['variation_' . $variation->nid]['tradingunit_' . $tradingunit->nid]['these_fields']['field_tu_deposit-' . $tradingunit->nid]['#default_value'] = str_replace('.', ',', $tradingunit->field_tu_deposit[LANGUAGE_NONE][0]['value']);
                    
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
                            'bag' => t('Bag'),
                        ),
                        '#title' => t('Trading unit type'),
                        '#required' => TRUE,
                        '#title_display' => 'none',
                        '#attributes' => array(
                            'placeholder' => t('Trading unit type'),
                            'class' => array('form-control'),
                            
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

function rm_seller_offer_form_validate($form, &$form_state) {
    foreach($form_state['values'] as $form_key => $form_value) {
        //Validate productunit amount
        if (!empty($form_state['values'][$form_key]) && strpos($form_key, 'field_productunit_amount-') !== false) {
            if(!is_numeric($form_value)) form_set_error($form_key, t('Please enter a numeric value as "Productunit Amount"'));
        }
        //Validate stock
        if (!empty($form_state['values'][$form_key]) && strpos($form_key, 'field_stock-') !== false) {
            if(isset($form_state['values'][$form_key]) && !is_numeric($form_value)) form_set_error($form_key, t('Please enter a numeric value as stock'));
        }
        //Validate lead time
        if (!empty($form_state['values'][$form_key]) && strpos($form_key, 'field_leadtime-') !== false) {
            if(!is_numeric($form_value)) form_set_error($form_key, t('Please enter a numeric value as lead time'));
        }
        //Validate use within
        if (!empty($form_state['values'][$form_key]) && strpos($form_key, 'field_use_within-') !== false) {
            if(!is_numeric($form_value)) form_set_error($form_key, t('Please enter a numeric value as use within'));
        }
        //Validate TU Amount
        if (!empty($form_state['values'][$form_key]) && strpos($form_key, 'field_tu_amount-') !== false) {
            if(!is_numeric($form_value)) form_set_error($form_key, t('Please enter a numeric value as trading unit amount'));
        }
        //Validate TU Price
        if (!empty($form_state['values'][$form_key]) && strpos($form_key, 'field_tu_price-') !== false) {
            $form_state['values'][$form_key] = str_replace(',', '.', $form_value);
            if(!is_numeric($form_state['values'][$form_key])) form_set_error($form_key, t('Please enter a numeric value as trading unit price'));
        }
        //Validate TU RRP
        if (!empty($form_state['values'][$form_key]) && strpos($form_key, 'field_tu_rrp-') !== false) {
            $form_state['values'][$form_key] = str_replace(',', '.', $form_value);
            if(!is_numeric($form_state['values'][$form_key])) form_set_error($form_key, t('Please enter a numeric value as recommended retail price'));
        }
        //Validate TU Deposit
        if (!empty($form_state['values'][$form_key]) && strpos($form_key, 'field_tu_deposit-') !== false) {
            $form_state['values'][$form_key] = str_replace(',', '.', $form_value);
            if(!is_numeric($form_state['values'][$form_key])) form_set_error($form_key, t('Please enter a numeric value as trading unit deposit'));
        }
    }
}

function rm_seller_validate_date_empty($element, &$form_state, $form) {
  $date = $element['#value'];
  if (($date['year'] || $date['month'] || $date['day'])) {
    date_validate($element, $form_state, $form);
  }
}

function rm_seller_pre_render_date_empty(&$element) {

    // Don't put empty values if a date has already been set
    if (isset($element['#default_value'])) {  
        return $element;
    }

    // Loop through each field, add empty option and unset value
    foreach (array('year', 'month', 'day') as $field) {
        if (!isset($element[$field])) {
            continue;
        }

        unset($element[$field]['#value']);
        $element[$field]['#attributes']['class'][] = 'form-control';
        $element[$field]['#default_value'] = '0';
        $element[$field]['#options'] = array(
            '0' => '',
        ) + $element[$field]['#options'];
    }

    return $element;
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
                            node_save($nodeobject);
                            break;
                            
                        //Value fields with identical names
                        case 'field_sku':
                        case 'field_stock':
                        case 'field_gtin':
                        case 'field_leadtime':
                        case 'field_use_within':
                        case 'field_tu_amount':
                        case 'field_tu_price':
                        case 'field_tu_rrp':
                        case 'field_tu_vat':
                        case 'field_tu_deposit':
                        case 'field_tu_packaging':
                        case 'field_tu_vat':
                        case 'body':
                            $nodeobject->{$tmp[0]}[LANGUAGE_NONE][0]['value'] = $field_value;
                            node_save($nodeobject);
                            break;
                        
                        //Origin fields
                        case 'field_origin_company':
                            $nodeobject->field_origin[LANGUAGE_NONE][0]['organisation_name'] = $field_value;
                            node_save($nodeobject);
                            break;
                        case 'field_origin_zip':
                            $nodeobject->field_origin[LANGUAGE_NONE][0]['postal_code'] = $field_value;
                            node_save($nodeobject);
                            break;
                        case 'field_origin_locality':
                            $nodeobject->field_origin[LANGUAGE_NONE][0]['locality'] = $field_value;
                            node_save($nodeobject);
                            break;
                        case 'field_origin_country':
                            $nodeobject->field_origin[LANGUAGE_NONE][0]['country'] = $field_value;
                            node_save($nodeobject);
                            break;
                        
                        //Product unit
                        case 'field_productunit_amount':
                            $nodeobject->field_productunit[LANGUAGE_NONE][0]['first'] = $field_value;
                            node_save($nodeobject);
                            break;
                        case 'field_productunit_unit':
                            $nodeobject->field_productunit[LANGUAGE_NONE][0]['second'] = $field_value;
                            node_save($nodeobject);
                            break;
                        
                        //Image
                        case 'field_image':
                            $file = file_load($field_value);
                            $file->status = FILE_STATUS_PERMANENT;
                            file_save($file);
                            $filearray = (array) $file;
                            $nodeobject->{$tmp[0]}[LANGUAGE_NONE][] = (array) $file;
                            node_save($nodeobject);
                            break;
                        
                        //Expiration date
                        case 'field_expiration_date':
                            $timestamp = strtotime($field_value['year'] . '-' . $field_value['month'] . '-' . $field_value['day']);
                            $nodeobject->{$tmp[0]}[LANGUAGE_NONE][0]['value'] = $timestamp;
                            node_save($nodeobject);
                            break;
                        
                        //Add variation button
                        case 'add_variation_to':
                            drupal_goto('addoffervariation/' . $tmp[1], array('query' => drupal_get_destination()));
                            break;
                        
                        //Add tu button
                        case 'add_tu_to':
                            drupal_goto('addtradingunit/' . $tmp[1], array('query' => drupal_get_destination()));
                            break;
                    }
                }
            }
        }
    }
    
    drupal_set_message(t('All offers have been updated with the given values'), 'status');
}

function rm_seller_add_tradingunit($nid) {
    $variation = node_load($nid);
	$new_trading_unit = rm_api_create_new_node('trading_unit', node_type_get_name('trading_unit'), 'de', $variation->uid, 0, 0, 0, 0);
    $new_trading_unit->status = 1;
    node_save($new_trading_unit);
    $variation->field_trading_unit_reference[LANGUAGE_NONE][]['target_id'] = $new_trading_unit->nid;
    node_save($variation);
    drupal_goto();
}

function rm_seller_add_offervariation($nid) {
    $offerdescription = node_load($nid);
	$new_trading_unit = rm_api_create_new_node('trading_unit', node_type_get_name('trading_unit'), 'de', $offerdescription->uid, 0, 0, 0, 0);
    $new_trading_unit->status = 1;
    node_save($new_trading_unit);
	$new_offer_variation = rm_api_create_new_node('offer_variation', node_type_get_name('offer_variation'), 'de', $offerdescription->uid, 0, 0, 0, 0);
    $new_offer_variation->status = 1;
    $new_offer_variation->field_trading_unit_reference[LANGUAGE_NONE][]['target_id'] = $new_trading_unit->nid;
    node_save($new_offer_variation);
    $offerdescription->field_offer_variation_reference[LANGUAGE_NONE][]['target_id'] = $new_offer_variation->nid;
    node_save($offerdescription);
    drupal_goto();
}

function rm_seller_add_offerdescription($uid) {
	$new_trading_unit = rm_api_create_new_node('trading_unit', node_type_get_name('trading_unit'), 'de', $uid, 0, 0, 0, 0);
    $new_trading_unit->status = 1;
    node_save($new_trading_unit);
	$new_offer_variation = rm_api_create_new_node('offer_variation', node_type_get_name('offer_variation'), 'de', $uid, 0, 0, 0, 0);
    $new_offer_variation->status = 1;
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

function rm_seller_editaccount() {
    //pass to tpl
    module_load_include('inc', 'user', 'user.pages');
    global $user;
    $account = user_load($user->uid);
    return theme('rm_seller_theme_editaccount', array(
        'vars' => array(
            'form' => drupal_get_form('user_profile_form', $account),
        ),
    ));
}