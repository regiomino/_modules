<?php

function rm_sales_makeprospect($nid) {
    $nodeobject = node_load($nid);
    $nodeobject->prospect = 1;
    node_save($nodeobject);
    global $user;
    rm_user_activity(t('Marked @entry as prospect', array('@entry' => $nodeobject->title)), $nodeobject->nid, 'node', $user->uid);
    drupal_goto();
}

function rm_sales_removeprospect($nid) {
    $nodeobject = node_load($nid);
    $nodeobject->prospect = 0;
    node_save($nodeobject);
    global $user;
    rm_user_activity(t('Removed prospect marker from @entry', array('@entry' => $nodeobject->title)), $nodeobject->nid, 'node', $user->uid);
    drupal_goto();
}

function rm_sales_deletenode($form, &$form_state, $nid) {
	
	$nodeobject = node_load($nid);
	
	$form['rm_deleteprofile']['nid'] = array(
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

function rm_sales_deletenode_submit($form, &$form_state) {
	$title = node_load($form_state['values']['nid'])->title;
	node_delete($form_state['values']['nid']);
	global $user;
	drupal_set_message(t('Deleted the node @entry', array('@entry' => $title)), 'status');
	rm_user_activity(t('Deleted the node @entry', array('@entry' => $title)), NULL, NULL, $user->uid);
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
    rm_user_activity(t('Added offertable to @entry', array('@entry' => $new_node->title)), $new_node->nid, 'node', $user->uid);
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
		'#suffix' => l(t('Delete'), 'manage/sales/' . $agreement_id . '/delete', array('query' => drupal_get_destination())),
	);
	
	return $form;
}

function rm_sales_agreement_edit_validate($form, &$form_state) {
}

function rm_sales_agreement_edit_submit($form, &$form_state) {
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
                if(!is_array($values) || empty($values['value'])) unset($form_state['values'][$field_name][LANGUAGE_NONE][$index]);
            }
        }
		$form_state['values'][$field_name][LANGUAGE_NONE] = array_values($form_state['values'][$field_name][LANGUAGE_NONE]);
		$nodeobject->{$field_name} = $form_state['values'][$field_name];
	}
	node_save($nodeobject);
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



function rm_sales_note_add($form, &$form_state, $nid) {

	$form['rm_add_note'] = array();
	
	$form['title'] = array(
        '#type' => 'textfield',
        '#title' => t('Add note to @title', array('@title' => node_load($nid)->title)),
    );
    
    $form['nid'] = array(
        '#type' => 'value',
        '#value' => $nid,
    );
	
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save'),
	);
	
	return $form;
}

function rm_sales_note_add_validate($form, &$form_state) {
}

function rm_sales_note_add_submit($form, &$form_state) {
    global $user;
    $new_node = rm_api_create_new_node('note', $form_state['values']['title'], 'de', $user->uid, 1, 0, 0, 0);
    $new_node->field_entity_reference[LANGUAGE_NONE][0]['target_id'] = $form_state['values']['nid'];
    node_save($new_node);
    drupal_set_message(t('Note was saved'), 'status');
    drupal_goto();
}

function rm_sales_useraccounts_form($form, &$form_state) {
    $form = array();
    
    global $user;
    
    $header = array(
		'status' => array(
			'data' => t('Status'),
			'type' => 'property',
			'specifier' => 'status',
		),
		'uid' => array(
			'data' => t('Customer number'),
			'type' => 'property',
			'specifier' => 'uid',
			'sort' => 'desc',
		),
		'field_gender' => array(
			'data' => t('Salutation'),
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_gender',
				'column' => 'value',
			),
		),
		'name' => array(
			'data' => t('Name'),
			'type' => 'property',
			'specifier' => 'name',
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
        'actions' => t('Actions'),
	);
    
    if(!rm_user_is_admin()) {
        $query = new EntityFieldQuery();
        $tmp = $query
            ->entityCondition('entity_type', 'node')
            ->entityCondition('bundle', array('seller_profile', 'customer_profile'))
            ->fieldCondition('field_salesguy', 'target_id', $user->uid)
            ->execute();
        $alluids = array();
        if(isset($tmp['node'])) {
            $allprofiles = entity_load('node', array_keys($tmp['node']));
            if(isset($allprofiles)) {
                foreach($allprofiles as $nid => $profile) {
                    $alluids[$profile->uid] = TRUE;
                }
            }
        }
    }
    
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'user');
    if(!rm_user_is_admin()) $query->propertyCondition('uid', array_keys($alluids));
    $tmp = $query
		->tableSort($header)
		->pager(100)
		->execute();
        
    $alluseraccounts = array();
    
    //Remove anonymous user
    unset($tmp['user'][0]);
    if(!empty($tmp['user'])) {
        $alluseraccounts = entity_load('user', array_keys($tmp['user']));
        
        foreach($alluseraccounts as $uid => $userobject) {
        
            if($userobject->status) {
                $statusswitch = l(t('block'), 'user/' . $user->uid . '/disable/' . $uid, array('attributes' => array('class' => array('btn', 'btn-sm', 'btn-danger')), 'query' => drupal_get_destination()));
            }
            else {
                $statusswitch = l(t('unblock'), 'user/' . $user->uid . '/enable/' . $uid, array('attributes' => array('class' => array('btn', 'btn-sm', 'btn-warning')), 'query' => drupal_get_destination()));
            }
            

			$options[$uid] = array(
				'status' => ($userobject->status) ? t('Active') : t('Blocked'),
				'uid' => $uid,
                'field_gender' => (!empty($userobject->field_gender[LANGUAGE_NONE][0]['value'])) ? $userobject->field_gender[LANGUAGE_NONE][0]['value'] : '',
				'name' => format_username($userobject),
                'field_address_locality' => (!empty($userobject->field_address[LANGUAGE_NONE][0]['locality'])) ? $userobject->field_address[LANGUAGE_NONE][0]['locality'] : '',
                'field_address_zip' => (!empty($userobject->field_address[LANGUAGE_NONE][0]['postal_code'])) ? $userobject->field_address[LANGUAGE_NONE][0]['postal_code'] : '',
                'field_address_street' => (!empty($userobject->field_address[LANGUAGE_NONE][0]['thoroughfare'])) ? $userobject->field_address[LANGUAGE_NONE][0]['thoroughfare'] : '',
                'actions' => l(t('Shadow log in'), 'user/' . $user->uid . '/shadow/' . $uid, array('attributes' => array('class' => array('btn', 'btn-sm', 'btn-info')))) . ' ' . $statusswitch,
			);
		}
		
		$form['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Do stuff'),
		);
		
		$form['useraccounts'] = array(
			'#type' => 'tableselect',
			'#header' => $header,
			'#options' => $options,
			'#attributes' => array(),
		);
		
		$form['#header'] = $header;
		
		$form['pager'] = array('#markup' => theme('pager'));
        
    }
    
    return $form;
}

function rm_sales_editprofile($form, &$form_state, $nid) {
	$profileobject = node_load($nid);
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

function rm_sales_publishprofile($nid) {
    $validation = rm_api_node_validate_required($nid);
	if(empty($validation)) {
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
        
        global $user;
        rm_user_activity(
            t('Published profile <a href="@url">@profile</a>', array('@url' => url('node/' . $nodeobject->nid), '@profile' => $nodeobject->title)),
            $nodeobject->nid,
            'node',
            $user->uid
        );
        
        drupal_set_message(t('Profile <a href="@url">@profile</a> has been published', array('@url' => url('node/' . $nodeobject->nid), '@profile' => $nodeobject->title)), 'status');
        
    }
    else {
        drupal_set_message(t('Profile <a href="@url">@profile</a> has not been published due to a validation error', array('@url' => url('node/' . $nodeobject->nid), '@profile' => $nodeobject->title)), 'error');
    }
	drupal_goto();
}

function rm_sales_profilecare_form() {


    drupal_add_js(drupal_get_path('module', 'rm_sales') . '/js/rm_sales.js');

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
		'field_salesguy' => array(
			'data' => t('Salesguy'),
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_salesguy',
				'column' => 'target_id',
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
	$query
		->entityCondition('entity_type', 'node')
		->entityCondition('bundle', array('seller_profile'))
		->propertyCondition('status', 1);
    if(!rm_user_is_admin($user->uid)) $query->fieldCondition('field_salesguy', 'target_id', $user->uid);
	$tmp = $query
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
			
			$shipping_agreement = '<div class="list-group">';
            $agrnodes = rm_api_get_nodes_by_properties(array('shipping_agreement'), 1, -1, -1, -1, -1, $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id']);
			if(isset($agrnodes)) {
				foreach($agrnodes as $agrnid => $agrnode) {
					$agrtitle = date('d.m.Y', $agrnode->changed) . ' ';
					if($agrnode->field_user_reference[LANGUAGE_NONE][0]['target_id'] > 0) {
                        $userobject = user_load($agrnode->field_user_reference[LANGUAGE_NONE][0]['target_id']);
						$agrtitle .= t('for @user', array('@user' => format_username($userobject)));
					}
					else {
						$agrtitle .= t('for all');
					}
					$shipping_agreement .= l($agrtitle, 'manage/sales/editagreement/' . $agrnode->nid, array('query' => drupal_get_destination(), 'attributes' => array('class' => array('list-group-item'))));
				}
			}
			$shipping_agreement .= l('<span class="glyphicon glyphicon-plus"></span> ' . t('add new one'), 'manage/sales/addagreement/shipping_agreement/' . $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id'], array('query' => drupal_get_destination(), 'html' => TRUE, 'attributes' => array('class' => array('list-group-item', 'active')))) . '</div>';
			
			$dispatch_agreement = '<div class="list-group">';
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
					$dispatch_agreement .= l($agrtitle, 'manage/sales/editagreement/' . $agrnode->nid, array('query' => drupal_get_destination(), 'attributes' => array('class' => array('list-group-item'))));
				}
			}
			$dispatch_agreement .= l('<span class="glyphicon glyphicon-plus"></span> ' . t('add new one'), 'manage/sales/addagreement/dispatch_agreement/' . $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id'], array('query' => drupal_get_destination(), 'html' => TRUE, 'attributes' => array('class' => array('list-group-item', 'active')))) . '</div>';
			
			
			$pickup_agreement = '<div class="list-group">';
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
					$pickup_agreement .= l($agrtitle, 'manage/sales/editagreement/' . $agrnode->nid, array('query' => drupal_get_destination(), 'attributes' => array('class' => array('list-group-item'))));
				}
			}
			$pickup_agreement .= l('<span class="glyphicon glyphicon-plus"></span> ' . t('add new one'), 'manage/sales/addagreement/pickup_agreement/' . $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id'], array('query' => drupal_get_destination(), 'html' => TRUE, 'attributes' => array('class' => array('list-group-item', 'active')))) . '</div>';
			
			$commission_agreement = '<div class="list-group">';
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
					$commission_agreement .= l($agrtitle, 'manage/sales/editagreement/' . $agrnode->nid, array('query' => drupal_get_destination(), 'attributes' => array('class' => array('list-group-item'))));
				}
			}
			$commission_agreement .= l('<span class="glyphicon glyphicon-plus"></span> ' . t('add new one'), 'manage/sales/addagreement/commission_agreement/' . $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id'], array('query' => drupal_get_destination(), 'html' => TRUE, 'attributes' => array('class' => array('list-group-item', 'active')))) . '</div>';
			
			$payment_agreement = '<div class="list-group">';
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
					$payment_agreement .= l($agrtitle, 'manage/sales/editagreement/' . $agrnode->nid, array('query' => drupal_get_destination(), 'attributes' => array('class' => array('list-group-item'))));
				}
			}
			$payment_agreement .= l('<span class="glyphicon glyphicon-plus"></span> ' . t('add new one'), 'manage/sales/addagreement/payment_agreement/' . $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id'], array('query' => drupal_get_destination(), 'html' => TRUE, 'attributes' => array('class' => array('list-group-item', 'active')))) . '</div>';
			
			$offer_table = '<div class="list-group">';
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
					$offer_table .= l($agrtitle, file_create_url($agrnode->field_offer_table[LANGUAGE_NONE][0]['uri']), array('attributes' => array('class' => array('list-group-item'))));
				}
			}
			$offer_table .= l('<span class="glyphicon glyphicon-plus"></span> ' . t('add new one'), 'manage/sales/addoffertable/' . $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id'], array('query' => drupal_get_destination(), 'html' => TRUE, 'attributes' => array('class' => array('list-group-item', 'active')))) . '</div>';
            
            $salesguynid = format_username($user);
            if(rm_user_is_admin($user->uid)) {
                $salesguynid = '<div id="salesguys"><select id="salesguy_' . $nid . '" class="form-control input-sm">';
                $salesguys = entity_load('user', rm_user_get_users_by_role('salesguy'));
                foreach($salesguys as $uid => $userobject) {
                    $salesguynid .= '<option value="' . $uid . '"';
                    if ($profileobject->field_salesguy[LANGUAGE_NONE][0]['target_id'] == $uid) $salesguynid .= ' selected';
                    $salesguynid .= '>' . format_username($userobject) . '</option>';
                }
                $salesguynid .= '</select></div>';
            }
			
			$options[$nid] = array(
				'type' => node_type_get_name($profileobject),
				'title' => $profileobject->title . '<br />' . l(t('edit'), 'manage/sales/' . $nid . '/edit', array('query' => drupal_get_destination(), 'attributes' => array('class' => array('btn', 'btn-sm', 'btn-warning')))),
				'field_address_locality' => $locality,
				'field_address_zip' => $zip,
				'field_address_street' => $street,
				'field_publicphone' => $publicphone,
                'field_salesguy' => $salesguynid,
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

function rm_sales_profileset_form() {

	global $user;
    
    drupal_add_js(drupal_get_path('module', 'rm_sales') . '/js/rm_sales.js');
	
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
		'field_next_action' => array(
			'data' => t('Nächste Aktion'),
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_next_action',
				'column' => 'value',
			),
		),
		'field_salescomment' => array(
			'data' => t('Comment'),
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_salescomment',
				'column' => 'value',
			),
		),
		'field_priority' => array(
			'data' => t('Priority'),
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_priority',
				'column' => 'value',
			),
		),
    );
    
    if(rm_user_is_admin($user->uid)) {
        $header['field_salesguy'] = array(
			'data' => t('Salesguy'),
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_salesguy',
				'column' => 'target_id',
			),
		);
    }
    
    $header['notes'] = t('Notes');
    $header['actions'] = t('Actions');
	

	$query = new EntityFieldQuery();
	$query
		->entityCondition('entity_type', 'node')
		->entityCondition('bundle', array('customer_profile', 'seller_profile'))
		->propertyCondition('status', 0)
		->propertyCondition('sticky', 1);
    if(!rm_user_is_admin($user->uid)) $query->fieldCondition('field_salesguy', 'target_id', $user->uid);
    $query
		->tableSort($header)
		->pager(100);
    $tmp = $query->execute();
		
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
			if(empty($publishingpossible)) $publishingpossiblestring = ' ' . l(t('Publish'), 'manage/sales/' . $nid . '/publish', array('query' => drupal_get_destination(), 'attributes' => array('class' => array('btn', 'btn-sm', 'btn-success'))));
            
            
            if(rm_user_is_admin($user->uid)) {
                $salesguynid = '<div class="salesguys"><select id="salesguy_' . $nid . '" class="form-control input-sm">';
                $salesguys = entity_load('user', rm_user_get_users_by_role('salesguy'));
                foreach($salesguys as $uid => $userobject) {
                    $salesguynid .= '<option value="' . $uid . '"';
                    if ($profileobject->field_salesguy[LANGUAGE_NONE][0]['target_id'] == $uid) $salesguynid .= ' selected';
                    $salesguynid .= '>' . format_username($userobject) . '</option>';
                }
                $salesguynid .= '</select></div>';
            }
            
            $ntqry = new EntityFieldQuery();
            $nttmp = $ntqry
                ->entityCondition('entity_type', 'node')
                ->entityCondition('bundle', 'note')
                ->propertyCondition('status', 1)
                ->fieldCondition('field_entity_reference', 'target_id', $nid)
                ->propertyOrderBy('created', 'DESC')
                ->execute();
            $allnotes = array();
            if(isset($nttmp['node'])) $allnotes = entity_load('node', array_keys($nttmp['node']));
            $notes = '';
            
            if(!empty($allnotes)) {
                $notes = l(t('Notes'), '#', array('external' => TRUE, 'attributes' => array('class' => array('btn', 'btn-sm', 'btn-primary'), 'data-toggle' => 'modal', 'data-target' => '#noteModal' . $nid))) . '
                    
                    <div class="modal fade" id="noteModal' . $nid . '">
                        <div class="modal-dialog">
                            <div class="modal-content">
                                <div class="modal-body"><ul class="list-group">';
                foreach($allnotes as $notenid => $noteobject) {
                    $notes .= '<li class="list-group-item"><span class="badge">' . date('d.m.Y', $noteobject->created) . ' - ' . format_username(user_load($noteobject->uid));
                    if(rm_user_is_admin()) $notes .= ' - (' . l('X', 'manage/sales/' . $notenid . '/delete', array('query' => drupal_get_destination())) . ')';
                    $notes .= '</span>' . $noteobject->title . '</li>';
                }
                $notes .= '
                                </ul></div>
                            </div>
                        </div>
                    </div>';
			}
            
            $prio_field = field_info_field('field_priority');
            $prio_allowed_values = list_allowed_values($prio_field);
            $prionid = '<div class="priorities"><select id="priority_' . $nid . '" class="form-control input-sm" style="width: 70px;">';
            foreach($prio_allowed_values as $value => $valdesc) {
                $prionid .= '<option value="' . $value . '"';
                if ($profileobject->field_priority[LANGUAGE_NONE][0]['value'] == $value) $prionid .= ' selected';
                $prionid .= '>' . $valdesc . '</option>';
            }
            $prionid .= '</select></div>';
            
            $salescomment = '<div class="comments"><textarea id="comment_' . $nid . '" class="form-control">';
            if(isset($profileobject->field_salescomment[LANGUAGE_NONE][0]['value'])) $salescomment .= $profileobject->field_salescomment[LANGUAGE_NONE][0]['value'];
            $salescomment .= '</textarea></div>';
            
            $nextaction = '';
            if(!empty($profileobject->field_next_action[LANGUAGE_NONE][0]['value'])) $nextaction = date('Y-m-d', $profileobject->field_next_action[LANGUAGE_NONE][0]['value']);
            
			$options[$nid] = array(
                'prospect' => ($profileobject->prospect) ? t('Yes') : t('No'),
                //'suggester' => (!empty($profileobject->field_suggester[LANGUAGE_NONE])) ? count($profileobject->field_suggester[LANGUAGE_NONE]) : 0,
				'type' => node_type_get_name($profileobject),
				'title' => $profileobject->title,
				'field_address_locality' => $locality,
				'field_address_zip' => $zip,
				'field_address_street' => $street,
				'field_publicphone' => $publicphone,
                'field_next_action' => '<div class="nextactions"><input id="nextaction_' . $nid . '" class="form-control" type="date" value="' . $nextaction . '" /></div>',
                'field_salescomment' => $salescomment,
                'field_priority' => $prionid,
                'notes' => $notes,
                'actions' => l(t('Complete profile'), 'manage/sales/' . $nid . '/edit', array('query' => drupal_get_destination(), 'attributes' => array('class' => array('btn', 'btn-sm', 'btn-warning')))) . $publishingpossiblestring . ' ' . l(t('Delete'), 'manage/sales/' . $nid . '/delete', array('query' => drupal_get_destination(), 'attributes' => array('class' => array('btn', 'btn-sm', 'btn-danger')))) . ' ' . l(($profileobject->prospect) ? t('Remove prospect') : t('Make prospect'), ($profileobject->prospect) ? 'manage/sales/removeprospect/' . $nid : 'manage/sales/makeprospect/' . $nid, array('query' => drupal_get_destination(), 'attributes' => array('class' => array('btn', 'btn-sm', 'btn-info')))) . ' ' . l(t('Change to "@nodetype"', array('@nodetype' => node_type_get_name($nodetypename))), 'manage/sales/changeprofiletype/' . $nid . '/' . $nodetypename, array('query' => drupal_get_destination(), 'attributes' => array('class' => array('btn', 'btn-sm', 'btn-default')))) . ' ' . l(t('Add note'), 'manage/sales/addnote/' . $nid, array('query' => drupal_get_destination(), 'attributes' => array('class' => array('btn', 'btn-sm', 'btn-primary')))),
			);
            
            if(rm_user_is_admin($user->uid)) {
				$options[$nid]['field_salesguy'] = $salesguynid;
            }
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
			->execute();
		$loadarray = array_keys($tmp['node']);
	}
	$allprofiles = entity_load('node', $loadarray);
	$header = array(
		t('Profiletype'),
        t('Prospect'),
        t('Step'),
        t('Completed profile'),
		t('Name'),
		t('Street'),
		t('Zip'),
		t('Locality'),
		t('Phone'),
		t('Salesguy'),
		t('Shipping agreement'),
		t('Dispatch agreement'),
		t('Pickup agreement'),
		t('Commission agreement'),
		t('Payment agreement'),
		t('Offer table'),
        t('Notes'),
	);
	$rows = array();
	foreach($allprofiles as $nid => $profileobject) {
		$zip = '';
		if(!empty($profileobject->field_address[LANGUAGE_NONE][0]['postal_code'])) $zip = $profileobject->field_address[LANGUAGE_NONE][0]['postal_code'];
		$phone = '';
		if(!empty($profileobject->field_publicphone[LANGUAGE_NONE][0]['number'])) $phone = $profileobject->field_publicphone[LANGUAGE_NONE][0]['number'];
        $step = 1;
        if($profileobject->sticky == 1 && $profileobject->status == 0) $step = 2;
        if($profileobject->status == 1) $step = 3;
        $valid = rm_api_node_validate_required($nid);
        
        if(isset($profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id'])) {
            $shipnodes = rm_api_get_nodes_by_properties(array('shipping_agreement'), 1, -1, -1, -1, -1, $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id']);
            $disnodes = rm_api_get_nodes_by_properties(array('dispatch_agreement'), 1, -1, -1, -1, -1, $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id']);
            $picknodes = rm_api_get_nodes_by_properties(array('pickup_agreement'), 1, -1, -1, -1, -1, $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id']);
            $commnodes = rm_api_get_nodes_by_properties(array('commission_agreement'), 1, -1, -1, -1, -1, $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id']);
            $paynodes = rm_api_get_nodes_by_properties(array('payment_agreement'), 1, -1, -1, -1, -1, $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id']);
            $ofrtbl = rm_api_get_nodes_by_properties(array('offer_table'), 1, -1, -1, -1, -1, $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id']);
        }
        
        
        $ntqry = new EntityFieldQuery();
        $nttmp = $ntqry
            ->entityCondition('entity_type', 'node')
            ->entityCondition('bundle', 'note')
            ->propertyCondition('status', 1)
            ->fieldCondition('field_entity_reference', 'target_id', $nid)
            ->propertyOrderBy('created', 'DESC')
            ->execute();
        $allnotes = array();
        if(isset($nttmp['node'])) $allnotes = entity_load('node', array_keys($nttmp['node']));
        $notes = '';
        
        if(!empty($allnotes)) {
            foreach($allnotes as $notenid => $noteobject) {
                $notes .= '[' . date('d.m.Y', $noteobject->created) . ' - ' . format_username(user_load($noteobject->uid)) . ' - ' . $noteobject->title . '] ';
            }
        }
        
        
        
        $username = format_username(user_load($profileobject->field_salesguy[LANGUAGE_NONE][0]['target_id']));
		$rows[] = array(
			node_type_get_name($profileobject->type),
            $profileobject->prospect,
            $step,
            (empty($valid)) ? t('Yes') : t('No'),
			$profileobject->title,
			$profileobject->field_address[LANGUAGE_NONE][0]['thoroughfare'],
			$zip,
			$profileobject->field_address[LANGUAGE_NONE][0]['locality'],
			$phone,
            $username,
            (isset($shipnodes)) ? t('Yes') : t('No'),
            (isset($disnodes)) ? t('Yes') : t('No'),
            (isset($picknodes)) ? t('Yes') : t('No'),
            (isset($commnodes)) ? t('Yes') : t('No'),
            (isset($paynodes)) ? t('Yes') : t('No'),
            (isset($ofrtbl)) ? t('Yes') : t('No'),
            $notes,
		);
	}
	rm_api_excel_export($header, $rows);
}


function rm_sales_profilequeue_form() {

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
                'type' => node_type_get_name($profileobject),
				'title' => $profileobject->title,
				'field_address_locality' => $locality,
				'field_address_zip' => $zip,
				'field_address_street' => $street,
				'field_publicphone' => $publicphone,
				'actions' => l(t('Take over'), 'manage/sales/assignprofile/' . $nid . '/' . $user->uid, array('query' => drupal_get_destination(), 'attributes' => array('class' => array('btn', 'btn-sm', 'btn-success')))) . ' ' . l(t('Delete'), 'manage/sales/' . $nid . '/delete', array('query' => drupal_get_destination(), 'attributes' => array('class' => array('btn', 'btn-sm', 'btn-danger')))) . ' ' . l(($profileobject->prospect) ? t('Remove prospect') : t('Make prospect'), ($profileobject->prospect) ? 'manage/sales/removeprospect/' . $nid : 'manage/sales/makeprospect/' . $nid, array('query' => drupal_get_destination(), 'attributes' => array('class' => array('btn', 'btn-sm', 'btn-info')))) . ' ' . l(t('Change to "@nodetype"', array('@nodetype' => node_type_get_name($nodetypename))), 'manage/sales/changeprofiletype/' . $nid . '/' . $nodetypename, array('query' => drupal_get_destination(), 'attributes' => array('class' => array('btn', 'btn-sm', 'btn-default')))),
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

function rm_sales_profilequeue_form_validate($form, &$form_state) {
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

function rm_sales_profilequeue_form_submit($form, &$form_state) {
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

function rm_sales_changepriority($nid, $priority) {
	$profileobject = node_load($nid);
	$profileobject->field_priority[LANGUAGE_NONE][0]['value'] = $priority;
	node_save($profileobject);
	global $user;
	$userobject = user_load($uid);
	rm_user_activity(
		t('Changed priority on <a href="@url">@profile</a> to @prio', array('@url' => url('node/' . $nid), '@profile' => $profileobject->title, '@prio' => $priority)),
		$nid,
		'node',
		$user->uid
	);
	drupal_goto();
}

function rm_sales_updatecomment($nid) {
    $text = $_POST['text'];
    $profileobject = node_load($nid);
	$profileobject->field_salescomment[LANGUAGE_NONE][0]['value'] = $text;
	node_save($profileobject);
	global $user;
	$userobject = user_load($uid);
	rm_user_activity(
		t('Updated comment on <a href="@url">@profile</a>', array('@url' => url('node/' . $nid), '@profile' => $profileobject->title)),
		$nid,
		'node',
		$user->uid
	);
	drupal_goto();
}

function rm_sales_updatenextaction($nid) {
    $text = $_POST['text'];
    $profileobject = node_load($nid);
	$profileobject->field_next_action[LANGUAGE_NONE][0]['value'] = strtotime($text);
	node_save($profileobject);
	global $user;
	$userobject = user_load($uid);
	rm_user_activity(
		t('Updated next action date on <a href="@url">@profile</a>', array('@url' => url('node/' . $nid), '@profile' => $profileobject->title)),
		$nid,
		'node',
		$user->uid
	);
	drupal_goto();
}

function rm_sales_press() {
    //pass to tpl
    return theme('rm_sales_theme_press', array(
        'vars' => array(
            'reports' => rm_api_get_nodes_by_properties(array('press_report'), 1, -1, -1, -1, -1, -1, 'DESC'),
            'logo' => node_load(1393),
        ),
    ));
}