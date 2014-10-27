<?php

/**
 * Form builder.
 * @ingroup forms
 * @see system_settings_form()
 */
 
function rm_cart_admin_cart_preservation() {

	//Cart preservation. Maximum number of days, inactive cart items should be preserved
	$form['cart_values']['regiomino_max_cart_days'] = array(
		'#type' => 'textfield',
		'#title' => t('Cart preservation'),
		'#description' => t('Please enter the maximum number of days, inactive cart items should be preserved. If a cart item becomes older than this, it will be unpublished with the next cronjob.'),
		'#maxlength' => 3,
		'#size' => 1,
		'#default_value' => variable_get('regiomino_max_cart_days', 1),
		'#field_suffix' => t('day(s)'),
	);

	//Process form with system_settings_form
    return system_settings_form($form);
}