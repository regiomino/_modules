<?php

function rm_user_process_applications() {
    watchdog('indeed', print_r($_POST, TRUE));
    watchdog('indeed', print_r($_REQUEST, TRUE));
    print "hello world";
}

function rm_user_myaccount($form, &$form_state, $node_type) {

	global $user;
	
	$header = array(
		'created' => array(
			'data' => t('Date'),
			'type' => 'property',
			'specifier' => 'created',
			'sort' => 'desc',
		),
		'body' => array(
			'data' => t('Reason'),
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_reason',
				'column' => 'value',
			),
		),
		'field_amount' => array(
			'data' => t('Amount'),
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_amount',
				'column' => 'value',
			),
		),
		'field_balance' => array(
			'data' => t('Balance'),
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_balance',
				'column' => 'value',
			),
		),
	);
	

		$query = new EntityFieldQuery();
		$tmp = $query
			->entityCondition('entity_type', 'node')
			->entityCondition('bundle', $node_type)
			->propertyCondition('uid', $user->uid)
			->tableSort($header)
			->pager(50)
			->execute();

	$form = array();
	
	if(isset($tmp['node'])) {
		$transactions = entity_load('node', array_keys($tmp['node']));
		$options = array();
		foreach($transactions as $nid => $nodeobject) {

			$options[$nid] = array(
				'created' => date('d.m.Y H:i', $nodeobject->created),
				'body' => $nodeobject->body[LANGUAGE_NONE][0]['value'],
				'field_amount' => number_format((double)$nodeobject->field_amount[LANGUAGE_NONE][0]['value'], 2, ",", ".") . ' €',
				'field_balance' => number_format((double)$nodeobject->field_balance[LANGUAGE_NONE][0]['value'], 2, ",", ".") . ' €',
			);
		}
		$form['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Do stuff'),
		);
		
		$form['transactions'] = array(
			'#type' => 'tableselect',
			'#header' => $header,
			'#options' => $options,
			'#attributes' => array(),
		);
		
		$form['#header'] = $header;
		
		$form['pager'] = array('#markup' => theme('pager'));
	}
	else {
		drupal_set_message(t('You have no transactions so far'), 'warning');
	}
	return $form;
}

function rm_user_editprofile($form, &$form_state, $account) {

    global $user;
    $profilenodes = rm_api_get_nodes_by_properties(array('seller_profile', 'customer_profile'), -1, -1, -1, -1, -1, $user->uid);
    $profilekeys = array_keys($profilenodes);
	$profileobject = $profilenodes[$profilekeys[0]];
    
	$nid = $profileobject->nid;
    
	$form = array();
	 
	$form['company'] = array(
		'#type' => 'fieldset',
		'#title' => t('Company information'),
	);
 
	rm_api_attach_field_to_form('field_company_name', 'node', $profileobject->type, $profileobject, $form['company'], $form_state, LANGUAGE_NONE);
	 
	rm_api_attach_field_to_form('field_publicphone', 'node', $profileobject->type, $profileobject, $form['company'], $form_state, LANGUAGE_NONE);
	 
	rm_api_attach_field_to_form('field_publicfax', 'node', $profileobject->type, $profileobject, $form['company'], $form_state, LANGUAGE_NONE);
	
	rm_api_attach_field_to_form('field_email', 'node', $profileobject->type, $profileobject, $form['company'], $form_state, LANGUAGE_NONE);
	
	rm_api_attach_field_to_form('field_bankaccountholder', 'node', $profileobject->type, $profileobject, $form['company'], $form_state, LANGUAGE_NONE);
	
	rm_api_attach_field_to_form('field_iban', 'node', $profileobject->type, $profileobject, $form['company'], $form_state, LANGUAGE_NONE);
	
	rm_api_attach_field_to_form('field_bic', 'node', $profileobject->type, $profileobject, $form['company'], $form_state, LANGUAGE_NONE);
	
	rm_api_attach_field_to_form('field_taxnumber', 'node', $profileobject->type, $profileobject, $form['company'], $form_state, LANGUAGE_NONE);
	
	rm_api_attach_field_to_form('field_kleinunternehmer', 'node', $profileobject->type, $profileobject, $form['company'], $form_state, LANGUAGE_NONE);

	rm_api_attach_field_to_form('field_billingaddress', 'node', $profileobject->type, $profileobject, $form['company'], $form_state, LANGUAGE_NONE);

	rm_api_attach_field_to_form('field_address', 'node', $profileobject->type, $profileobject, $form['company'], $form_state, LANGUAGE_NONE);
	
	$form['profile'] = array(
		'#type' => 'fieldset',
		'#title' => t('Profile information'),
	);
	
	$form['profile']['title'] = array(
		'#type' => 'textfield',
		'#required' => TRUE,
		'#attributes' => array('placeholder' => t('Title of the profile')),
		'#title' => t('Profile title'),
	);
    
	if($profileobject->type == 'seller_profile') rm_api_attach_field_to_form('field_sellercategories', 'node', $profileobject->type, $profileobject, $form['profile'], $form_state, LANGUAGE_NONE);
    
	
	$form['profile']['nid'] = array(
		'#type' => 'value',
		'#value' => $nid,
	);
	
	if(!empty($profileobject->title)) $form['profile']['title']['#default_value'] = $profileobject->title;
	
	rm_api_attach_field_to_form('field_image', 'node', $profileobject->type, $profileobject, $form['profile'], $form_state, LANGUAGE_NONE);
	
	rm_api_attach_field_to_form('body', 'node', $profileobject->type, $profileobject, $form['profile'], $form_state, LANGUAGE_NONE);
	
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Submit'),
	);
	
	return $form;
}

function rm_user_editprofile_validate($form, &$form_state) {

}

function rm_user_editprofile_submit($form, &$form_state) {
	$nodeobject = node_load($form_state['values']['nid']);
	
	$nodeobject->title = $form_state['values']['title'];
	$nodeobject->field_company_name = $form_state['values']['field_company_name'];
	$nodeobject->field_billingaddress = $form_state['values']['field_billingaddress'];
	$nodeobject->field_address = $form_state['values']['field_address'];
	$nodeobject->field_publicphone = $form_state['values']['field_publicphone'];
	$nodeobject->field_publicfax = $form_state['values']['field_publicfax'];
	$nodeobject->field_email = $form_state['values']['field_email'];
	$nodeobject->field_bankaccountholder = $form_state['values']['field_bankaccountholder'];
	$nodeobject->field_iban = $form_state['values']['field_iban'];
	$nodeobject->field_bic = $form_state['values']['field_bic'];
	$nodeobject->field_taxnumber = $form_state['values']['field_taxnumber'];
	$nodeobject->field_kleinunternehmer = $form_state['values']['field_kleinunternehmer'];	
	$nodeobject->field_image = $form_state['values']['field_image'];	
	if(isset($nodeobject->field_sellercategories)) $nodeobject->field_sellercategories = $form_state['values']['field_sellercategories'];
	$nodeobject->body = $form_state['values']['body'];
	node_save($nodeobject);
	global $user;
	rm_user_activity(
		t('Completed profile <a href="@url">@profile</a>', array('@url' => url('node/' . $nodeobject->nid), '@profile' => $nodeobject->title)),
		$nodeobject->nid,
		'node',
		$user->uid
	);
}