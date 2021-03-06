<?php

function rm_user_deleteuser($form, &$form_state, $account) {

	$form['rm_deleteuser']['uid'] = array(
		'#type' => 'value',
		'#value' => $account->uid,
	);
	
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Delete'),
		'#prefix' => '<p>' . t('Do you really want to delete the user "@user" and all his data (incl. - if exists - profile, products, agreements, etc.)?', array('@user' => $account->name)) . '</p>',
		'#suffix' => l(t('Cancel'), $_GET['destination']),
	);
	
	return $form;
}

function rm_user_deleteuser_submit($form, &$form_state) {
    $query = new EntityFieldQuery();
    $result = $query
        ->entityCondition('entity_type', 'node')
        ->propertyCondition('uid', $form_state['values']['uid'])
        ->execute();

    $nodes = array();
    if(!empty($result['node'])) {
        foreach($result['node'] as $nid => $bla) {
            node_delete($nid);
            drupal_set_message(t('Deleted node @node', array('@node' => $nid)), 'status');
        }
    }
    user_delete($form_state['values']['uid']);
	drupal_set_message(t('Deleted user @user', array('@user' => $form_state['values']['uid'])), 'status');
    global $user;
	rm_user_activity(t('Deleted user @user', array('@user' => $userobject->name)), NULL, NULL, $user->uid);
}

function rm_user_orders() {
    global $user;
    $orders = rm_cart_get_orders_grouped_by_order_number($user->uid, array(), 0, 'customer');
    return theme('rm_user_theme_orders', array(
        'vars' => array(
            'orders' => $orders,
        ),
    ));
}

function rm_user_dashboard() {
    global $user;
    $profiles = rm_api_get_nodes_by_properties(array('customer_profile', 'trader_profile'), 1, -1, -1, -1, -1, $user->uid);
    $profile = array();
    $profilecompleteness = 0;
    if(!empty($profiles)) {
        $profileskeys = array_keys($profiles);
        $profile = $profiles[$profileskeys[0]];
        $profilecompleteness = rm_user_get_profile_completeness($profile->nid, 'seller_profile');
    }
    $sortedorders = rm_cart_get_orders_grouped_by_order_number($user->uid, array('processing', 'accepted', 'delivered'), 0, 'customer');
    //pass to tpl
    return theme('rm_user_theme_dashboard', array(
        'vars' => array(
            'profile' => $profile,
            'profilecompleteness' => $profilecompleteness,
            'accountcompleteness' => rm_user_get_account_completeness($user->uid),
            'orders' => $sortedorders,
            'ordercount' => count($sortedorders),
        ),
    ));
}


function rm_user_profile_registration($form, $form_state, $profile) {
    
    $profileobject = node_load($profile);
    $account = user_load($profileobject->uid);
    
    $form = array();
    
    $form['profileregistration']['gender'] = array(
        '#type' => 'select',
        '#title' => t('Salutation'),
        '#options' => array(
            'f' => 'Frau',
            'm' => 'Herr',
        ),
        '#required' => TRUE,
    );
    if(!empty($account->field_gender[LANGUAGE_NONE][0]['value'])) {
        $form['profileregistration']['gender']['#default_value'] = $account->field_gender[LANGUAGE_NONE][0]['value'];
    }
    
    $form['profileregistration']['first_name'] = array(
        '#type' => 'textfield',
        '#title' => t('First name'),
        '#required' => TRUE,
    );
    if(!empty($account->field_first_name[LANGUAGE_NONE][0]['value'])) {
        $form['profileregistration']['first_name']['#default_value'] = $account->field_first_name[LANGUAGE_NONE][0]['value'];
    }
    
    $form['profileregistration']['last_name'] = array(
        '#type' => 'textfield',
        '#title' => t('Last name'),
        '#required' => TRUE,
    );
    if(!empty($account->field_last_name[LANGUAGE_NONE][0]['value'])) {
        $form['profileregistration']['last_name']['#default_value'] = $account->field_last_name[LANGUAGE_NONE][0]['value'];
    }
    
    $form['profileregistration']['profile_title'] = array(
        '#type' => 'textfield',
        '#title' => t('Profile title'),
        '#required' => TRUE,
    );
    if(!empty($profileobject->title)) {
        $form['profileregistration']['profile_title']['#default_value'] = $profileobject->title;
    }
    
    $form['profileregistration']['company_name'] = array(
        '#type' => 'textfield',
        '#title' => t('Company name'),
        '#required' => TRUE,
    );
    if(!empty($profileobject->field_company_name[LANGUAGE_NONE][0]['value'])) {
        $form['profileregistration']['company_name']['#default_value'] = $profileobject->field_company_name[LANGUAGE_NONE][0]['value'];
    }
    
    $form['profileregistration']['smallbusiness'] = array(
        '#type' => 'checkbox',
        '#title' => t('I am not obliged to pay value-added tax according to § 19 UStG.'),
    );
    if(!empty($profileobject->field_kleinunternehmer[LANGUAGE_NONE][0]['value'])) {
        $form['profileregistration']['smallbusiness']['#default_value'] = $profileobject->field_kleinunternehmer[LANGUAGE_NONE][0]['value'];
    }
    
    $form['profileregistration']['phone'] = array(
        '#type' => 'textfield',
        '#title' => t('Phone'),
        '#field_prefix' => '+49',
        '#required' => TRUE,
    );
    if(!empty($profileobject->field_publicphone[LANGUAGE_NONE][0]['number'])) {
        $form['profileregistration']['phone']['#default_value'] = $profileobject->field_publicphone[LANGUAGE_NONE][0]['number'];
    }
    
    $form['profileregistration']['fax'] = array(
        '#type' => 'textfield',
        '#title' => t('Fax'),
        '#field_prefix' => '+49',
    );
    if(!empty($profileobject->field_publicfax[LANGUAGE_NONE][0]['number'])) {
        $form['profileregistration']['fax']['#default_value'] = $profileobject->field_publicfax[LANGUAGE_NONE][0]['number'];
    }
    
    $form['profileregistration']['email'] = array(
        '#type' => 'textfield',
        '#title' => t('Email'),
        '#required' => TRUE,
    );
    if(!empty($profileobject->field_email[LANGUAGE_NONE][0]['email'])) {
        $form['profileregistration']['email']['#default_value'] = $profileobject->field_email[LANGUAGE_NONE][0]['email'];
    }
    
    $form['profileregistration']['iban'] = array(
        '#type' => 'textfield',
        '#title' => t('IBAN'),
        '#required' => TRUE,
    );
    if(!empty($profileobject->field_iban[LANGUAGE_NONE][0]['value'])) {
        $form['profileregistration']['iban']['#default_value'] = $profileobject->field_iban[LANGUAGE_NONE][0]['value'];
    }
    
    $form['profileregistration']['bic'] = array(
        '#type' => 'textfield',
        '#title' => t('BIC'),
        '#required' => TRUE,
    );
    if(!empty($profileobject->field_bic[LANGUAGE_NONE][0]['value'])) {
        $form['profileregistration']['bic']['#default_value'] = $profileobject->field_bic[LANGUAGE_NONE][0]['value'];
    }
    
    $form['profileregistration']['accountholder'] = array(
        '#type' => 'textfield',
        '#title' => t('Bank account holder'),
        '#required' => TRUE,
    );
    if(!empty($profileobject->field_bankaccountholder[LANGUAGE_NONE][0]['value'])) {
        $form['profileregistration']['accountholder']['#default_value'] = $profileobject->field_bankaccountholder[LANGUAGE_NONE][0]['value'];
    }
    
    $form['profileregistration']['taxnumber'] = array(
        '#type' => 'textfield',
        '#title' => t('Tax ID'),
        '#required' => TRUE,
    );
    if(!empty($profileobject->field_taxnumber[LANGUAGE_NONE][0]['value'])) {
        $form['profileregistration']['taxnumber']['#default_value'] = $profileobject->field_taxnumber[LANGUAGE_NONE][0]['value'];
    }
    
    $form['profileregistration']['street'] = array(
        '#type' => 'textfield',
        '#title' => t('Street'),
        '#required' => TRUE,
    );
    if(!empty($profileobject->field_address[LANGUAGE_NONE][0]['thoroughfare'])) {
        $form['profileregistration']['street']['#default_value'] = $profileobject->field_address[LANGUAGE_NONE][0]['thoroughfare'];
    }
    
    $form['profileregistration']['zipcode'] = array(
        '#type' => 'textfield',
        '#required' => TRUE,
        '#title' => t('Postal code'),
    );
    if(!empty($profileobject->field_address[LANGUAGE_NONE][0]['postal_code'])) {
        $form['profileregistration']['zipcode']['#default_value'] = $profileobject->field_address[LANGUAGE_NONE][0]['postal_code'];
    }
    
    $form['profileregistration']['locality'] = array(
        '#type' => 'textfield',
        '#title' => t('Locality'),
        '#required' => TRUE,
    );
    if(!empty($profileobject->field_address[LANGUAGE_NONE][0]['locality'])) {
        $form['profileregistration']['locality']['#default_value'] = $profileobject->field_address[LANGUAGE_NONE][0]['locality'];
    }
    
    $form['profileregistration']['newsletter'] = array(
        '#type' => 'checkbox',
        '#title' => '<strong>' . t('I would like to subscribe to the Regiomino Newsletter') . '</strong>',
    );
    
    $form['profileregistration']['profile_id'] = array(
        '#type' => 'value',
        '#value' => $profileobject->nid,
    );

    $form['profileregistration']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Register'),
        '#suffix' => t('By clicking @register I accept the <a href="@url" target="_blank">General Terms and Conditions</a>', array('@register' => t('Register'), '@url' => url('node/4'))),
    );
    return $form;
}

function rm_user_profile_registration_submit($form, $form_state) {
    $profileobject = node_load($form_state['values']['profile_id']);
    $account = user_load($profileobject->uid);
    
    //Save account field values
    $account->field_gender[LANGUAGE_NONE][0]['value'] = $form_state['values']['gender'];
    $account->field_first_name[LANGUAGE_NONE][0]['value'] = $form_state['values']['first_name'];
    $account->field_last_name[LANGUAGE_NONE][0]['value'] = $form_state['values']['last_name'];
    $account->mail = $form_state['values']['email'];
    $account->field_address[LANGUAGE_NONE][0]['thoroughfare'] = $form_state['values']['street'];
    $account->field_address[LANGUAGE_NONE][0]['postal_code'] = $form_state['values']['zipcode'];
    $account->field_address[LANGUAGE_NONE][0]['locality'] = $form_state['values']['locality'];
    
    //Save profile field values
    $profileobject->title = $form_state['values']['profile_title'];
    $profileobject->field_company_name[LANGUAGE_NONE][0]['value'] = $form_state['values']['company_name'];
    $profileobject->field_publicphone[LANGUAGE_NONE][0]['number'] = $form_state['values']['phone'];
    if(!empty($form_state['values']['fax'])) $profileobject->field_publicfax[LANGUAGE_NONE][0]['number'] = $form_state['values']['fax'];
    $profileobject->field_email[LANGUAGE_NONE][0]['email'] = $form_state['values']['email'];
    $profileobject->field_iban[LANGUAGE_NONE][0]['value'] = $form_state['values']['iban'];
    $profileobject->field_bic[LANGUAGE_NONE][0]['value'] = $form_state['values']['bic'];
    $profileobject->field_bankaccountholder[LANGUAGE_NONE][0]['value'] = $form_state['values']['accountholder'];
    $profileobject->field_taxnumber[LANGUAGE_NONE][0]['value'] = $form_state['values']['taxnumber'];
    if(!empty($form_state['values']['smallbusiness'])) $profileobject->field_kleinunternehmer[LANGUAGE_NONE][0]['value'] = $form_state['values']['smallbusiness'];
    $profileobject->field_billingaddress[LANGUAGE_NONE][0]['thoroughfare'] = $form_state['values']['street'];
    $profileobject->field_billingaddress[LANGUAGE_NONE][0]['postal_code'] = $form_state['values']['zipcode'];
    $profileobject->field_billingaddress[LANGUAGE_NONE][0]['locality'] = $form_state['values']['locality'];
    $profileobject->field_address[LANGUAGE_NONE][0]['thoroughfare'] = $form_state['values']['street'];
    $profileobject->field_address[LANGUAGE_NONE][0]['postal_code'] = $form_state['values']['zipcode'];
    $profileobject->field_address[LANGUAGE_NONE][0]['locality'] = $form_state['values']['locality'];
    node_save($profileobject);
    
    rm_user_has_accepted_gtc($form_state['values']['profile_id']);
    
    if(!empty($form_state['values']['newsletter'])) {
        $result = rm_api_mailchimp_subscribe(
            $form_state['values']['email'],
            array(
                'FNAME' => $form_state['values']['first_name'],
                'LNAME' => $form_state['values']['last_name'],
                'SALUTATION' => ($form_state['values']['gender'] == 'f') ? 'Frau' : 'Herr',
                'ROLES' => 'authenticated user'
            ),
            FALSE,
            TRUE,
            TRUE,
            FALSE,
            '042acaef72'
        );
    }
    
    drupal_set_message(t('Thank you for your registration. You will receive an email shortly with your login details and a summary of your registration information. If you wish to change anything, please log in and use your dashboard to do so.'), 'status');
    drupal_goto('user/register');
}

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
    $profilenodes = rm_api_get_nodes_by_properties(array('seller_profile', 'customer_profile', 'trader_profile'), -1, -1, -1, -1, -1, $user->uid);
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
    usort($form_state['values']['field_image'][LANGUAGE_NONE], 'rm_user_usort_image_by_weight');
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