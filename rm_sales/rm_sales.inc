<?php

function rm_sales_makeprospect($nid) {
    $nodeobject = node_load($nid);
    $nodeobject->prospect = 1;
    node_save($nodeobject);
    drupal_goto();
}

function rm_sales_removeprospect($nid) {
    $nodeobject = node_load($nid);
    $nodeobject->prospect = 0;
    node_save($nodeobject);
    drupal_goto();
}

function rm_sales_deleteprofile($form, &$form_state, $nid) {
	
	$nodeobject = node_load($nid);
	
	$form['rm_deleteprofile']['nid'] = array(
		'#type' => 'value',
		'#value' => $nid,
	);
	
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Delete'),
		'#prefix' => '<p>' . t('Do you really want to delete the profile "@profile"?', array('@profile' => $nodeobject->title)) . '</p>',
		'#suffix' => l(t('Cancel'), $_GET['destination']),
	);
	
	return $form;
}

function rm_sales_deleteprofile_submit($form, &$form_state) {
	$title = node_load($form_state['values']['nid'])->title;
	node_delete($form_state['values']['nid']);
	global $user;
	drupal_set_message(t('Deleted the profile @profile', array('@profile' => $title)), 'status');
	rm_user_activity(t('Deleted the profile @profile', array('@profile' => $title)), NULL, NULL, $user->uid);
}

function rm_sales_changeprofiletype($nid, $nodetype) {
	$oldtype = node_type_get_name(node_load($nid)->type);
	rm_api_change_node_type($nid, $nodetype);
	global $user;
	rm_user_activity(t('Changed the profiletype of @nodetitle from @oldtype to @newtype', array('@nodetitle' => node_load($nid)->title, '@oldtype' => $oldtype, '@newtype' => node_type_get_name($nodetype))), $nid, 'node', $user->uid);
	drupal_goto();
}

function rm_sales_offertable_add($form, &$form_state, $suid) {
	$instances = field_info_instances('node', 'offer_table');

	$form['rm_add_offertable'] = array();
	
	foreach($instances as $field_name => $field_description) {
		rm_api_attach_field_to_form($field_name, 'node', 'offer_table', NULL, $form['rm_add_offertable'], $form_state, LANGUAGE_NONE);
	}
	
	$form['rm_add_offertable']['suid'] = array(
		'#type' => 'value',
		'#value' => $suid,
	);
	
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save'),
	);
	
	return $form;
}

function rm_sales_offertable_add_validate($form, &$form_state) {
}

function rm_sales_offertable_add_submit($form, &$form_state) {
	$title = node_type_get_name('offer_table');
	$new_node = rm_api_create_new_node('offer_table', $title, 'de', $form_state['values']['suid'], 1, 0, 0, 0);
	if(!empty($form_state['values']['body'][LANGUAGE_NONE][0]['value'])) $new_node->body = $form_state['values']['body'];
	$new_node->field_offer_table = $form_state['values']['field_offer_table'];
	node_save($new_node);
}

function rm_sales_agreement_edit($form, &$form_state, $agreement_id) {
	
	$agreement = node_load($agreement_id);
	
	$instances = field_info_instances('node', $agreement->type);

	$form['rm_edit_agreement'] = array();
	
	foreach($instances as $field_name => $field_description) {
		rm_api_attach_field_to_form($field_name, 'node', $agreement->type, $agreement, $form['rm_edit_agreement'], $form_state, LANGUAGE_NONE);
	}
	
	$form['rm_add_agreement']['field_names'] = array(
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
	);
	
	return $form;
}

function rm_sales_agreement_edit_validate($form, &$form_state) {
}

function rm_sales_agreement_edit_submit($form, &$form_state) {
	$nodeobject = node_load($form_state['values']['nid']);
	
	foreach($form_state['values']['field_names'] as $field_name => $field_definitions) {
		foreach($form_state['values'][$field_name][LANGUAGE_NONE] as $index => $dayvalues) {
			if($field_name == 'field_regular_times') {
				if($dayvalues['starthours'] == '' || $dayvalues['endhours'] = '') unset($form_state['values'][$field_name][LANGUAGE_NONE][$index]);
			}
			if(!is_numeric($index)) unset($form_state['values'][$field_name][LANGUAGE_NONE][$index]);
		}
		$form_state['values'][$field_name][LANGUAGE_NONE] = array_values($form_state['values'][$field_name][LANGUAGE_NONE]);
		if($field_name == 'field_user_reference') $form_state['values'][$field_name][LANGUAGE_NONE][0]['target_id'] = 0;
		$nodeobject->{$field_name} = $form_state['values'][$field_name];
	}
	
	node_save($nodeobject);
	global $user;
}

function rm_sales_agreement_add($form, &$form_state, $agreement_type, $suid) {

	$instances = field_info_instances('node', $agreement_type);

	$form['rm_add_agreement'] = array();
	
	foreach($instances as $field_name => $field_description) {
		rm_api_attach_field_to_form($field_name, 'node', $agreement_type, NULL, $form['rm_add_agreement'], $form_state, LANGUAGE_NONE);
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

function rm_sales_agreement_add_validate($form, &$form_state) {
}

function rm_sales_agreement_add_submit($form, &$form_state) {	
	$title = node_type_get_name($form_state['values']['agreement_type']);
	$new_node = rm_api_create_new_node($form_state['values']['agreement_type'], $title, 'de', $form_state['values']['suid'], 1, 0, 0, 0);
	foreach($form_state['values']['field_names'] as $field_name => $field_definitions) {
		foreach($form_state['values'][$field_name][LANGUAGE_NONE] as $index => $dayvalues) {
			if($field_name == 'field_regular_times') {
				if($dayvalues['starthours'] == '' || $dayvalues['endhours'] = '') unset($form_state['values'][$field_name][LANGUAGE_NONE][$index]);
			}
			if(!is_numeric($index)) unset($form_state['values'][$field_name][LANGUAGE_NONE][$index]);
		}
		$form_state['values'][$field_name][LANGUAGE_NONE] = array_values($form_state['values'][$field_name][LANGUAGE_NONE]);
		if($field_name == 'field_user_reference') $form_state['values'][$field_name][LANGUAGE_NONE][0]['target_id'] = 0;
		$new_node->{$field_name} = $form_state['values'][$field_name];
	}
	node_save($new_node);
}

function rm_sales_editprofile($form, &$form_state, $nid) {
	$profileobject = node_load($nid);
	
	$form = array();
	 
	$form['rm_profile_edit']['company'] = array(
		'#type' => 'fieldset',
		'#title' => t('Company information'),
	);
	
	rm_api_attach_field_to_form('field_company_name', 'node', $profileobject->type, $profileobject, $form['rm_profile_edit']['company'], $form_state, LANGUAGE_NONE);
	
	rm_api_attach_field_to_form('field_billingaddress', 'node', $profileobject->type, $profileobject, $form['rm_profile_edit']['company'], $form_state, LANGUAGE_NONE);
	 
	rm_api_attach_field_to_form('field_address', 'node', $profileobject->type, $profileobject, $form['rm_profile_edit']['company'], $form_state, LANGUAGE_NONE);
	 
	rm_api_attach_field_to_form('field_publicphone', 'node', $profileobject->type, $profileobject, $form['rm_profile_edit']['company'], $form_state, LANGUAGE_NONE);
	 
	rm_api_attach_field_to_form('field_publicfax', 'node', $profileobject->type, $profileobject, $form['rm_profile_edit']['company'], $form_state, LANGUAGE_NONE);
	
	rm_api_attach_field_to_form('field_email', 'node', $profileobject->type, $profileobject, $form['rm_profile_edit']['company'], $form_state, LANGUAGE_NONE);
	
	rm_api_attach_field_to_form('field_bankaccountholder', 'node', $profileobject->type, $profileobject, $form['rm_profile_edit']['company'], $form_state, LANGUAGE_NONE);
	
	rm_api_attach_field_to_form('field_iban', 'node', $profileobject->type, $profileobject, $form['rm_profile_edit']['company'], $form_state, LANGUAGE_NONE);
	
	rm_api_attach_field_to_form('field_bic', 'node', $profileobject->type, $profileobject, $form['rm_profile_edit']['company'], $form_state, LANGUAGE_NONE);
	
	rm_api_attach_field_to_form('field_taxnumber', 'node', $profileobject->type, $profileobject, $form['rm_profile_edit']['company'], $form_state, LANGUAGE_NONE);
	
	rm_api_attach_field_to_form('field_kleinunternehmer', 'node', $profileobject->type, $profileobject, $form['rm_profile_edit']['company'], $form_state, LANGUAGE_NONE);

	
	$form['rm_profile_edit']['profile'] = array(
		'#type' => 'fieldset',
		'#title' => t('Profile information'),
	);
	
	$form['rm_profile_edit']['profile']['title'] = array(
		'#type' => 'textfield',
		'#required' => TRUE,
		'#attributes' => array('placeholder' => t('Title of the profile')),
		'#title' => t('Profile title'),
	);
	
	$form['rm_profile_edit']['profile']['nid'] = array(
		'#type' => 'value',
		'#value' => $nid,
	);
	
	if(!empty($profileobject->title)) $form['rm_profile_edit']['profile']['title']['#default_value'] = $profileobject->title;
	
	rm_api_attach_field_to_form('field_image', 'node', $profileobject->type, $profileobject, $form['rm_profile_edit']['profile'], $form_state, LANGUAGE_NONE);
	
	rm_api_attach_field_to_form('body', 'node', $profileobject->type, $profileobject, $form['rm_profile_edit']['profile'], $form_state, LANGUAGE_NONE);
	
	$form['rm_profile_edit']['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Submit'),
	);
	
	return $form;
}

function rm_sales_editprofile_validate($form, &$form_state) {

}

function rm_sales_editprofile_submit($form, &$form_state) {
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

function rm_sales_publishprofile($nid) {

	if(rm_api_node_validate_required($nid)) {
		$nodeobject = node_load($nid);
		
		$userexists = user_load_by_mail($nodeobject->field_email[LANGUAGE_NONE][0]['email']);
		if(!$userexists) {
			//create user for registree
			$password = user_password(8);
			//set up the user fields
			$fields = array(
				'name' => $nodeobject->field_email[LANGUAGE_NONE][0]['email'],
				'mail' => $nodeobject->field_email[LANGUAGE_NONE][0]['email'],
				'pass' => $password,
				'status' => 1,
				'init' => $nodeobject->field_email[LANGUAGE_NONE][0]['email'],
				'roles' => array(
					DRUPAL_AUTHENTICATED_RID => 'authenticated user',
					'4' => 'seller',
				),
			);
			//the first parameter is left blank so a new user is created
			$account = user_save('', $fields);
			$account->password = $fields['pass'];
			$uid = $account->uid;
            $result = rm_api_mailchimp_subscribe(
                $nodeobject->field_email[LANGUAGE_NONE][0]['email'],
                array(
                    'FNAME' => $nodeobject->title,
                    'LNAME' => $nodeobject->field_address[LANGUAGE_NONE][0]['locality'],
                    'SALUTATION' => 'Herr',
                    'ROLES' => 'authenticated user, seller'
                )
            );
			//drupal_mail('rm_user', 'profile_published_user_registered', $nodeobject->field_email[LANGUAGE_NONE][0]['email'], NULL, array('account' => $account, 'password' => $password), variable_get('site_mail', 'support@regiomino.de'));
		}
		else {
			$uid = $userexists->uid;
		}
		
		$nodeobject->uid = $uid;
		$nodeobject->field_user_reference[LANGUAGE_NONE][0]['target_id'] = $uid;
		node_save($nodeobject);
		
		rm_api_node_publish($nid);
	}
	global $user;
	rm_user_activity(
		t('Published profile <a href="@url">@profile</a>', array('@url' => url('node/' . $nodeobject->nid), '@profile' => $nodeobject->title)),
		$nodeobject->nid,
		'node',
		$user->uid
	);
	
	drupal_set_message(t('Profile <a href="@url">@profile</a> has been published', array('@url' => url('node/' . $nodeobject->nid), '@profile' => $nodeobject->title)), 'status');
	drupal_goto();
}

function rm_sales_profilecare() {

	global $user;
	
	$header = array(
		'type' => array(
			'data' => t('Profile type'),
			'type' => 'property',
			'specifier' => 'type',
			'sort' => 'desc',
		),
		'title' => array(
			'data' => t('Name'),
			'type' => 'property',
			'specifier' => 'title',
			'sort' => 'desc',
		),
		'field_address_locality' => array(
			'data' => t('City'),
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_address',
				'column' => 'locality',
			),
		),
		'field_address_zip' => array(
			'data' => t('Zip'),
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_address',
				'column' => 'postal_code',
			),
		),
		'field_address_street' => array(
			'data' => t('Street'),
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_address',
				'column' => 'thoroughfare',
			),
		),
		'field_publicphone' => array(
			'data' => t('Phone'),
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_publicphone',
				'column' => 'number',
			),
		),
		'shipping_agreement' => node_type_get_name('shipping_agreement'),
		'dispatch_agreement' => node_type_get_name('dispatch_agreement'),
		'pickup_agreement' => node_type_get_name('pickup_agreement'),
		'commission_agreement' => node_type_get_name('commission_agreement'),
		'payment_agreement' => node_type_get_name('payment_agreement'),
		'offer_table' => node_type_get_name('offer_table'),
	);
	

	$query = new EntityFieldQuery();
	$tmp = $query
		->entityCondition('entity_type', 'node')
		->entityCondition('bundle', array('customer_profile', 'seller_profile'))
		->propertyCondition('status', 1)
		->fieldCondition('field_salesguy', 'target_id', $user->uid)
		->tableSort($header)
		->pager(100)
		->execute();
		
	$form = array();
	
	if(isset($tmp['node'])) {
		$queue = entity_load('node', array_keys($tmp['node']));
		
		$options = array();
		foreach($queue as $nid => $profileobject) {
		
			$locality = '';
			if(!empty($profileobject->field_address[LANGUAGE_NONE][0]['locality'])) {
				$locality = $profileobject->field_address[LANGUAGE_NONE][0]['locality'];
			}
			
			$zip = '';
			if(!empty($profileobject->field_address[LANGUAGE_NONE][0]['postal_code'])) {
				$zip = $profileobject->field_address[LANGUAGE_NONE][0]['postal_code'];
			}
			
			$street = '';
			if(!empty($profileobject->field_address[LANGUAGE_NONE][0]['thoroughfare'])) {
				$street = $profileobject->field_address[LANGUAGE_NONE][0]['thoroughfare'];
			}
		
			$publicphone = '';
			if(!empty($profileobject->field_publicphone[LANGUAGE_NONE][0]['number'])) {
				$publicphone = '+49 - ' . $profileobject->field_publicphone[LANGUAGE_NONE][0]['number'];
			}
			
			$shipping_agreement = '<ul>';
            $agrnodes = rm_api_get_nodes_by_properties(array('shipping_agreement'), 1, -1, -1, -1, -1, $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id']);
			if(isset($agrnodes)) {
				foreach($agrnodes as $agrnid => $agrnode) {
					$agrtitle = date('d.m.Y', $agrnode->changed) . ' ';
					if(!empty($agrnode->field_user_reference[LANGUAGE_NONE][0]['target_id'])) {
						$agrtitle .= $userobject->mail;
					}
					else {
						$agrtitle .= t('for all');
					}
					$shipping_agreement .= '<li>' . l($agrtitle, 'admin/sales/editagreement/' . $agrnode->nid, array('query' => drupal_get_destination())) . '</li>';
				}
			}
			$shipping_agreement .= '</ul><em>' . l(t('add new one'), 'admin/sales/addagreement/shipping_agreement/' . $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id'], array('query' => drupal_get_destination())) . '</em>';
			
			$dispatch_agreement = '<ul>';
            $agrnodes = rm_api_get_nodes_by_properties(array('dispatch_agreement'), 1, -1, -1, -1, -1, $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id']);
            if(isset($agrnodes)) {
				foreach($agrnodes as $agrnid => $agrnode) {
					$agrtitle = date('d.m.Y', $agrnode->changed) . ' ';
					if(!empty($agrnode->field_user_reference[LANGUAGE_NONE][0]['target_id'])) {
						$agrtitle .= $userobject->mail;
					}
					else {
						$agrtitle .= t('for all');
					}
					$dispatch_agreement .= '<li>' . l($agrtitle, 'admin/sales/editagreement/' . $agrnode->nid, array('query' => drupal_get_destination())) . '</li>';
				}
			}
			$dispatch_agreement .= '</ul><em>' . l(t('add new one'), 'admin/sales/addagreement/dispatch_agreement/' . $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id'], array('query' => drupal_get_destination())) . '</em>';
			
			
			$pickup_agreement = '<ul>';
            $agrnodes = rm_api_get_nodes_by_properties(array('pickup_agreement'), 1, -1, -1, -1, -1, $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id']);
            if(isset($agrnodes)) {
				foreach($agrnodes as $agrnid => $agrnode) {
					$agrtitle = date('d.m.Y', $agrnode->changed) . ' ';
					if(!empty($agrnode->field_user_reference[LANGUAGE_NONE][0]['target_id'])) {
						$agrtitle .= $userobject->mail;
					}
					else {
						$agrtitle .= t('for all');
					}
					$pickup_agreement .= '<li>' . l($agrtitle, 'admin/sales/editagreement/' . $agrnode->nid, array('query' => drupal_get_destination())) . '</li>';
				}
			}
			$pickup_agreement .= '</ul><em>' . l(t('add new one'), 'admin/sales/addagreement/pickup_agreement/' . $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id'], array('query' => drupal_get_destination())) . '</em>';
			
			$commission_agreement = '<ul>';
            $agrnodes = rm_api_get_nodes_by_properties(array('commission_agreement'), 1, -1, -1, -1, -1, $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id']);
            if(isset($agrnodes)) {
				foreach($agrnodes as $agrnid => $agrnode) {
					$agrtitle = date('d.m.Y', $agrnode->changed) . ' ';
					if(!empty($agrnode->field_user_reference[LANGUAGE_NONE][0]['target_id'])) {
						$agrtitle .= $userobject->mail;
					}
					else {
						$agrtitle .= t('for all');
					}
					$commission_agreement .= '<li>' . l($agrtitle, 'admin/sales/editagreement/' . $agrnode->nid, array('query' => drupal_get_destination())) . '</li>';
				}
			}
			$commission_agreement .= '</ul><em>' . l(t('add new one'), 'admin/sales/addagreement/commission_agreement/' . $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id'], array('query' => drupal_get_destination())) . '</em>';
			
			$payment_agreement = '<ul>';
            $agrnodes = rm_api_get_nodes_by_properties(array('payment_agreement'), 1, -1, -1, -1, -1, $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id']);
            if(isset($agrnodes)) {
				foreach($agrnodes as $agrnid => $agrnode) {
					$agrtitle = date('d.m.Y', $agrnode->changed) . ' ';
					if(!empty($agrnode->field_user_reference[LANGUAGE_NONE][0]['target_id'])) {
						$agrtitle .= $userobject->mail;
					}
					else {
						$agrtitle .= t('for all');
					}
					$payment_agreement .= '<li>' . l($agrtitle, 'admin/sales/editagreement/' . $agrnode->nid, array('query' => drupal_get_destination())) . '</li>';
				}
			}
			$payment_agreement .= '</ul><em>' . l(t('add new one'), 'admin/sales/addagreement/payment_agreement/' . $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id'], array('query' => drupal_get_destination())) . '</em>';
			
			$offer_table = '<ul>';
			$agrqry = new EntityFieldQuery();
			$agrtmp = $agrqry
				->entityCondition('entity_type', 'node')
				->entityCondition('bundle', 'offer_table')
				->propertyCondition('status', 1)
				->propertyCondition('uid', $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id'])
				->propertyOrderBy('created', 'DESC')
				->range(0, 1)
				->execute();
			if(isset($agrtmp['node'])) {
				$agrnodes = entity_load('node', array_keys($agrtmp['node']));
				foreach($agrnodes as $agrnid => $agrnode) {
					$agrtitle = date('d.m.Y', $agrnode->changed) . ' ';
					if(!empty($agrnode->field_user_reference[LANGUAGE_NONE][0]['target_id'])) {
						$agrtitle .= $userobject->mail;
					}
					else {
						$agrtitle .= t('for all');
					}
					$offer_table .= '<li>' . l($agrtitle, file_create_url($agrnode->field_offer_table[LANGUAGE_NONE][0]['uri'])) . '</li>';
				}
			}
			$offer_table .= '</ul><em>' . l(t('add new one'), 'admin/sales/addoffertable/' . $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id'], array('query' => drupal_get_destination())) . '</em>';
			
			$options[$nid] = array(
				'type' => node_type_get_name($profileobject),
				'title' => $profileobject->title,
				'field_address_locality' => $locality,
				'field_address_zip' => $zip,
				'field_address_street' => $street,
				'field_publicphone' => $publicphone,
				'shipping_agreement' => $shipping_agreement,
				'dispatch_agreement' => $dispatch_agreement,
				'pickup_agreement' => $pickup_agreement,
				'commission_agreement' => $commission_agreement,
				'payment_agreement' => $payment_agreement,
				'offer_table' => $offer_table,
			);
		}
		
		$form['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Export to XLS'),
			'#submit' => array('rm_sales_export_profiles'),
		);
		
		$form['suggestions'] = array(
			'#type' => 'tableselect',
			'#header' => $header,
			'#options' => $options,
			'#attributes' => array(),
		);
		
		$form['#header'] = $header;
		
		$form['pager'] = array('#markup' => theme('pager'));
	}
	else {
		drupal_set_message(t('Currently there are no active profiles under your supervision. You should change that...'), 'warning');
	}

	return $form;
}

function rm_sales_profileset() {

	global $user;
	
	$header = array(
        'prospect' => array(
            'data' => t('Prospect'),
            'type' => 'property',
            'specifier' => 'prospect',
            'sort' => 'desc',
        ),
		'type' => array(
			'data' => t('Profile type'),
			'type' => 'property',
			'specifier' => 'type',
		),
		'title' => array(
			'data' => t('Name'),
			'type' => 'property',
			'specifier' => 'title',
		),
		'field_address_locality' => array(
			'data' => t('City'),
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_address',
				'column' => 'locality',
			),
		),
		'field_address_zip' => array(
			'data' => t('Zip'),
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_address',
				'column' => 'postal_code',
			),
		),
		'field_address_street' => array(
			'data' => t('Street'),
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_address',
				'column' => 'thoroughfare',
			),
		),
		'field_publicphone' => array(
			'data' => t('Phone'),
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_publicphone',
				'column' => 'number',
			),
		),
		'actions' => t('Actions'),
	);
	

	$query = new EntityFieldQuery();
	$tmp = $query
		->entityCondition('entity_type', 'node')
		->entityCondition('bundle', array('customer_profile', 'seller_profile'))
		->propertyCondition('status', 0)
		->propertyCondition('sticky', 1)
		->fieldCondition('field_salesguy', 'target_id', $user->uid)
		->tableSort($header)
		->pager(100)
		->execute();
		
	$form = array();
	
	if(isset($tmp['node'])) {
		$queue = entity_load('node', array_keys($tmp['node']));
		
		$options = array();
		foreach($queue as $nid => $profileobject) {
		
			$locality = '';
			if(!empty($profileobject->field_address[LANGUAGE_NONE][0]['locality'])) {
				$locality = $profileobject->field_address[LANGUAGE_NONE][0]['locality'];
			}
			
			$zip = '';
			if(!empty($profileobject->field_address[LANGUAGE_NONE][0]['postal_code'])) {
				$zip = $profileobject->field_address[LANGUAGE_NONE][0]['postal_code'];
			}
			
			$street = '';
			if(!empty($profileobject->field_address[LANGUAGE_NONE][0]['thoroughfare'])) {
				$street = $profileobject->field_address[LANGUAGE_NONE][0]['thoroughfare'];
			}
		
			$publicphone = '';
			if(!empty($profileobject->field_publicphone[LANGUAGE_NONE][0]['number'])) {
				$publicphone = '+49 - ' . $profileobject->field_publicphone[LANGUAGE_NONE][0]['number'];
			}
			
			switch($profileobject->type) {
				case "customer_profile": $nodetypename = 'seller_profile'; break;
				case "seller_profile": $nodetypename = 'customer_profile'; break;
			}
			
			
			$publishingpossiblestring = '';
			$publishingpossible = rm_api_node_validate_required($nid);
			if(isset($publishingpossible[0]['field_user_reference'])) unset($publishingpossible[0]);
			if(empty($publishingpossible)) $publishingpossiblestring = ' | ' . l(t('Publish'), 'admin/sales/' . $nid . '/publish', array('query' => drupal_get_destination()));

			
			$options[$nid] = array(
                'prospect' => ($profileobject->prospect) ? t('Yes') : t('No'),
				'type' => node_type_get_name($profileobject) . ' (' . l(t('Change to "@nodetype"', array('@nodetype' => node_type_get_name($nodetypename))), 'admin/sales/changeprofiletype/' . $nid . '/' . $nodetypename, array('query' => drupal_get_destination())) . ')',
				'title' => $profileobject->title,
				'field_address_locality' => $locality,
				'field_address_zip' => $zip,
				'field_address_street' => $street,
				'field_publicphone' => $publicphone,
				'actions' => l(t('Complete profile'), 'admin/sales/' . $nid . '/edit', array('query' => drupal_get_destination())) . $publishingpossiblestring . ' | ' . l(t('Delete'), 'admin/sales/' . $nid . '/delete', array('query' => drupal_get_destination())) . ' | ' . l(($profileobject->prospect) ? t('Remove prospect') : t('Make prospect'), ($profileobject->prospect) ? 'admin/sales/removeprospect/' . $nid : 'admin/sales/makeprospect/' . $nid, array('query' => drupal_get_destination())),
			);
		}
		
		$form['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Export to XLS'),
			'#submit' => array('rm_sales_export_profiles'),
		);
		
		$form['suggestions'] = array(
			'#type' => 'tableselect',
			'#header' => $header,
			'#options' => $options,
			'#attributes' => array(),
		);
		
		$form['#header'] = $header;
		
		$form['pager'] = array('#markup' => theme('pager'));
	}
	else {
		drupal_set_message(t('Currently there are no profiles assigned to you. You should change that...'), 'warning');
	}

	return $form;
}

function rm_sales_export_profiles($form, &$form_state) {
	$loadarray = array();
	foreach($form_state['values']['suggestions'] as $nid => $selected) {
		if($selected) $loadarray[] = $nid;
	}
	if(empty($loadarray)) {
		$query = new EntityFieldQuery();
		$tmp = $query
			->entityCondition('entity_type', 'node')
			->entityCondition('bundle', array('customer_profile', 'seller_profile'))
			->propertyCondition('sticky', 1)
			->execute();
		$loadarray = array_keys($tmp['node']);
	}
	$allprofiles = entity_load('node', $loadarray);
	$header = array(
		t('Profiletype'),
		t('Name'),
		t('Street'),
		t('Zip'),
		t('Locality'),
		t('Phone'),
	);
	$rows = array();
	foreach($allprofiles as $nid => $profileobject) {
		$zip = '';
		if(!empty($profileobject->field_address[LANGUAGE_NONE][0]['postal_code'])) $zip = $profileobject->field_address[LANGUAGE_NONE][0]['postal_code'];
		$phone = '';
		if(!empty($profileobject->field_publicphone[LANGUAGE_NONE][0]['number'])) $phone = $profileobject->field_publicphone[LANGUAGE_NONE][0]['number'];
	
		$rows[] = array(
			node_type_get_name($profileobject->type),
			$profileobject->title,
			$profileobject->field_address[LANGUAGE_NONE][0]['thoroughfare'],
			$zip,
			$profileobject->field_address[LANGUAGE_NONE][0]['locality'],
			$phone,
		);
	}
	rm_api_excel_export($header, $rows);
}

function rm_sales_profilequeue() {

	global $user;
	
	$header = array(
        'prospect' => array(
            'data' => t('Prospect'),
            'type' => 'property',
            'specifier' => 'prospect',
            'sort' => 'desc',
        ),
		'type' => array(
			'data' => t('Profile type'),
			'type' => 'property',
			'specifier' => 'type',
		),
		'title' => array(
			'data' => t('Name'),
			'type' => 'property',
			'specifier' => 'title',
		),
		'field_address_locality' => array(
			'data' => t('City'),
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_address',
				'column' => 'locality',
			),
		),
		'field_address_zip' => array(
			'data' => t('Zip'),
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_address',
				'column' => 'postal_code',
			),
		),
		'field_address_street' => array(
			'data' => t('Street'),
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_address',
				'column' => 'thoroughfare',
			),
		),
		'field_publicphone' => array(
			'data' => t('Phone'),
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_publicphone',
				'column' => 'number',
			),
		),
		'actions' => t('Actions'),
	);
	
	$query = new EntityFieldQuery();
	$tmp = $query
		->entityCondition('entity_type', 'user')
		->propertyCondition('status', 1)
		->execute();
	$users = entity_load('user', array_keys($tmp['user']));
	$salesguys = array();
	foreach($users as $uid => $userobject) {
		if(in_array('salesguy', $userobject->roles)) $salesguys[] = $uid;
	}
	$query = new EntityFieldQuery();
	$tmp = $query
		->entityCondition('entity_type', 'node')
		->entityCondition('bundle', array('customer_profile', 'seller_profile'))
		->propertyCondition('status', 0)
		->propertyCondition('sticky', 0)
		->tableSort($header)
		->pager(100)
		->execute();
		
	$form = array();
	
	if(isset($tmp['node'])) {
		$queue = entity_load('node', array_keys($tmp['node']));
		
		$options = array();
		foreach($queue as $nid => $profileobject) {
		
			$locality = '';
			if(!empty($profileobject->field_address[LANGUAGE_NONE][0]['locality'])) {
				$locality = $profileobject->field_address[LANGUAGE_NONE][0]['locality'];
			}
			
			$zip = '';
			if(!empty($profileobject->field_address[LANGUAGE_NONE][0]['postal_code'])) {
				$zip = $profileobject->field_address[LANGUAGE_NONE][0]['postal_code'];
			}
			
			$street = '';
			if(!empty($profileobject->field_address[LANGUAGE_NONE][0]['thoroughfare'])) {
				$street = $profileobject->field_address[LANGUAGE_NONE][0]['thoroughfare'];
			}
		
			$publicphone = '';
			if(!empty($profileobject->field_publicphone[LANGUAGE_NONE][0]['number'])) {
				$publicphone = '+49 - ' . $profileobject->field_publicphone[LANGUAGE_NONE][0]['number'];
			}
			
			switch($profileobject->type) {
				case "customer_profile": $nodetypename = 'seller_profile'; break;
				case "seller_profile": $nodetypename = 'customer_profile'; break;
			}
			
			$options[$nid] = array(
				'prospect' => ($profileobject->prospect) ? t('Yes') : t('No'),
                'type' => node_type_get_name($profileobject) . ' (' . l(t('Change to "@nodetype"', array('@nodetype' => node_type_get_name($nodetypename))), 'admin/sales/changeprofiletype/' . $nid . '/' . $nodetypename, array('query' => drupal_get_destination())) . ')',
				'title' => $profileobject->title,
				'field_address_locality' => $locality,
				'field_address_zip' => $zip,
				'field_address_street' => $street,
				'field_publicphone' => $publicphone,
				'actions' => l(t('Take over'), 'admin/sales/assignprofile/' . $nid . '/' . $user->uid, array('query' => drupal_get_destination())) . ' | ' . l(t('Delete'), 'admin/sales/' . $nid . '/delete', array('query' => drupal_get_destination())) . ' | ' . l(($profileobject->prospect) ? t('Remove prospect') : t('Make prospect'), ($profileobject->prospect) ? 'admin/sales/removeprospect/' . $nid : 'admin/sales/makeprospect/' . $nid, array('query' => drupal_get_destination())),
			);
		}
		
		$form['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Conflate selected suggestions'),
		);
		
		$form['suggestions'] = array(
			'#type' => 'tableselect',
			'#header' => $header,
			'#options' => $options,
			'#attributes' => array(),
		);
		
		$form['#header'] = $header;
		
		$form['pager'] = array('#markup' => theme('pager'));
	}
	else {
		drupal_set_message(t('Currently no unassigned profile suggestions exist. Good job! Now go get some more!'), 'warning');
	}

	return $form;
}

function rm_sales_profilequeue_validate($form, &$form_state) {
	//Check if at least two suggestions have been selected
	$counter = 0;
	$conflatable = array();
	foreach($form_state['values']['suggestions'] as $nid => $bool) {
		if($bool) {
			$counter++;
			$conflatable[] = $nid;
		}
	}
	if($counter < 2) form_set_error('suggestions', t('Please select at least two rows in order to conflate them'));
	$first = TRUE;
	foreach($conflatable as $nid) {
		$profileobject = node_load($nid);
		if($first) {
			$former = $profileobject->type;
			$first = FALSE;
		}
		$current = $profileobject->type;
		if($former != $current) {
			form_set_error('suggestions', t('The selected entries are not identical profile types. Please contact an administrator if you still wish to conflate them.'));
			break;
		}
		$former = $current;
	}
}

function rm_sales_profilequeue_submit($form, &$form_state) {
	$first = TRUE;
	foreach($form_state['values']['suggestions'] as $nid => $bool) {
		if($first) {
			$mainprofileobject = node_load($nid);
			$first = FALSE;
		}
		else {
			$profileobject = node_load($nid);
			if(
				(empty($mainprofileobject->field_address[LANGUAGE_NONE][0]['locality'])) &&
				(!empty($profileobject->field_address[LANGUAGE_NONE][0]['locality']))
			) {
				$mainprofileobject->field_address[LANGUAGE_NONE][0]['locality'] = $profileobject->field_address[LANGUAGE_NONE][0]['locality'];
			}
			if(
				(empty($mainprofileobject->field_address[LANGUAGE_NONE][0]['thoroughfare'])) &&
				(!empty($profileobject->field_address[LANGUAGE_NONE][0]['thoroughfare']))
			) {
				$mainprofileobject->field_address[LANGUAGE_NONE][0]['thoroughfare'] = $profileobject->field_address[LANGUAGE_NONE][0]['thoroughfare'];
			}
			if(
				(empty($mainprofileobject->field_address[LANGUAGE_NONE][0]['postal_code'])) &&
				(!empty($profileobject->field_address[LANGUAGE_NONE][0]['postal_code']))
			) {
				$mainprofileobject->field_address[LANGUAGE_NONE][0]['postal_code'] = $profileobject->field_address[LANGUAGE_NONE][0]['postal_code'];
			}
			if(
				(empty($mainprofileobject->field_publicphone[LANGUAGE_NONE][0]['number'])) &&
				(!empty($profileobject->field_publicphone[LANGUAGE_NONE][0]['number']))
			) {
				$mainprofileobject->field_publicphone[LANGUAGE_NONE][0]['number'] = $profileobject->field_publicphone[LANGUAGE_NONE][0]['number'];
				$mainprofileobject->field_publicphone[LANGUAGE_NONE][0]['country_codes'] = $profileobject->field_publicphone[LANGUAGE_NONE][0]['country_codes'];
			}
			if(!empty($profileobject->field_suggester[LANGUAGE_NONE][0]['number'])) {
				$mainprofileobject->field_suggester[LANGUAGE_NONE][] = array('value' => $profileobject->field_suggester[LANGUAGE_NONE][0]['value']);
			}
			node_save($mainprofileobject);
			node_delete($profileobject->nid);
		}
	}
	rm_user_activity(
		t('Conflated @count suggestions to one: <a href="@url">@profile</a>', array('@count' => count($form_state['values']['suggestions']), '@url' => url('node/' . $mainprofileobject->nid), '@profile' => $mainprofileobject->title)),
		$mainprofileobject->nid,
		'node'
		//$uid
	);
}

function rm_sales_assignprofile($nid, $uid) {
	$profileobject = node_load($nid);
	$profileobject->field_salesguy[LANGUAGE_NONE][0]['target_id'] = $uid;
	$profileobject->sticky = 1;
	node_save($profileobject);
	global $user;
	$userobject = user_load($uid);
	rm_user_activity(
		t('Assigned profile <a href="@url">@profile</a> to @targetuser', array('@url' => url('node/' . $nid), '@profile' => $profileobject->title, '@targetuser' => $userobject->mail)),
		$nid,
		'node',
		$user->uid
	);
	rm_user_activity(
		t('Was assigned profile <a href="@url">@profile</a> by @triggeruser', array('@triggeruser' => $user->uid, '@url' => url('node/' . $nid), '@profile' => $profileobject->title)),
		$nid,
		'node',
		$uid
	);
	drupal_set_message(t('The profile @profilename has been assigned to you', array('@profilename' => $profileobject->title)), 'status');
	drupal_goto();
}