<?php

function rm_sales_salesperformance() {

    drupal_add_js(drupal_get_path('module', 'rm_sales') . '/js/rm_sales_statistics.js');
    
    //Load all participants
    $query = new EntityFieldQuery();
    $tmp = $query
        ->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', array('seller_profile', 'customer_profile', 'trader_profile'))
        ->execute();
    $participants = array();
    if(!empty($tmp['node'])) {
        $participants = entity_load('node', array_keys($tmp['node']));
    }
    
    /**
     * Determine amount of registrations per month
     */
    $registrations = array();
    $invites = array();
    if(!empty($participants)) {
        foreach($participants as $participant) {
            if(rm_user_gtc_accepted($participant->nid)) {
                $registrations[strtotime(date('Y-m'))][] = TRUE;
            }
            else if(rm_user_invited($participant->nid)) {
                $invites[strtotime(date('Y-m'))][] = TRUE;
            }
        }
    }
    //Transfer to x/y-axis value pairs
    $flotRegistrations = 0;
    if(!empty($registrations)) {
        //Sort ascending by month
        ksort($registrations);
        foreach($registrations as $yearmonth => $booleans) {
            $amount = count($booleans);
            $flotRegistrations = $amount;
        }
    }
    //Transfer to x/y-axis value pairs
    $flotInvites = 0;
    if(!empty($invites)) {
        //Sort ascending by month
        ksort($invites);
        foreach($invites as $yearmonth => $booleans) {
            $amount = count($booleans);
            $flotInvites = $amount;
        }
    }
   
    
    //pass to tpl
    return theme('rm_sales_theme_salesperformance', array(
        'vars' => array(
            'registrations' => $flotRegistrations,
            'invites' => $flotInvites,
        ),
    ));
}

function rm_sales_salesguyactivity() {

	global $user;
	
	$header = array(
		'created' => array(
			'data' => t('Created'),
			'type' => 'property',
			'specifier' => 'created',
			'sort' => 'desc',
		),
		'uid' => array(
			'data' => t('User'),
			'type' => 'property',
			'specifier' => 'uid',
		),
        'body' => array(
			'data' => t('Activity'),
			'type' => 'field',
			'specifier' => array(
				'field' => 'body',
				'column' => 'value',
			),
		),
		'field_entityid' => array(
			'data' => t('Entity ID'),
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_entityid',
				'column' => 'value',
			),
		),
	);
	
    $usergroup1 = rm_user_get_users_by_role('salesguy');
    
        
    $queryparams = drupal_get_query_parameters();
    
	$query = new EntityFieldQuery();
	$query
		->entityCondition('entity_type', 'node')
		->entityCondition('bundle', array('activity_log'));
    
    //Filter injections
    if(!empty($queryparams['salesguy']) && $queryparams['salesguy'] > 0) {
        $query->propertyCondition('uid', $queryparams['salesguy']);
    }
    else {
        $query->propertyCondition('uid', $usergroup1);
    }
    
	$tmp = $query
		->tableSort($header)
		->pager(100)
		->execute();
    
    
	$filterform = drupal_get_form('rm_sales_salesguyactivity_filter_form');
		
	$form = array();
    
	if(isset($tmp['node'])) {
		$queue = entity_load('node', array_keys($tmp['node']));
		
		$options = array();
		foreach($queue as $nid => $activityobject) {
		
			$options[$nid] = array(
				'created' => date('d.m.Y H:i', $activityobject->created),
                'uid' => format_username(user_load($activityobject->uid)),
				'body' => $activityobject->body[LANGUAGE_NONE][0]['value'],
                'field_entityid' => (isset($activityobject->field_entitytype[LANGUAGE_NONE][0]['value']) && isset($activityobject->field_entityid[LANGUAGE_NONE][0]['value'])) ? l(t('Go to entity'), $activityobject->field_entitytype[LANGUAGE_NONE][0]['value'] . '/' . $activityobject->field_entityid[LANGUAGE_NONE][0]['value']) : '',
			);
            
            
		}
		
        
        $output = theme('table', array('header' => $header, 'rows' => $options, 'attributes' => array('class' => array('table'))));
	}
	else {
        $output = '';
		drupal_set_message(t('Currently there are no activity logs'), 'warning');
	}
    
	return render($filterform) . '<br /><div class="row">' . $output . theme('pager') . '</div>';
}

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

function rm_sales_suppress($nid) {
    $nodeobject = node_load($nid);
    $nodeobject->suppressed = 1;
    node_save($nodeobject);
    global $user;
    rm_user_activity(t('Marked @entry as suppressed', array('@entry' => $nodeobject->title)), $nodeobject->nid, 'node', $user->uid);
    drupal_goto();
}

function rm_sales_unsuppress($nid) {
    $nodeobject = node_load($nid);
    $nodeobject->suppressed = 0;
    node_save($nodeobject);
    global $user;
    rm_user_activity(t('Removed suppressed marker from @entry', array('@entry' => $nodeobject->title)), $nodeobject->nid, 'node', $user->uid);
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
    global $user;
	$title = node_type_get_name('offer_table');
	$new_node = rm_api_create_new_node('offer_table', $title, 'de', $form_state['values']['suid'], 1, 0, 0, 0);
	if(!empty($form_state['values']['body'][LANGUAGE_NONE][0]['value'])) $new_node->body = $form_state['values']['body'];
	$new_node->field_offer_table = $form_state['values']['field_offer_table'];
	node_save($new_node);
    rm_user_activity(t('Added offertable to @entry', array('@entry' => $new_node->title)), $new_node->nid, 'node', $user->uid);
}


function rm_sales_note_add($form, &$form_state, $nid) {

	$form['rm_add_note'] = array();
	
	$form['title'] = array(
        '#type' => 'textarea',
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
    $title = substr($form_state['values']['title'], 0, 120);
    $new_node = rm_api_create_new_node('note', $title, 'de', $user->uid, 1, 0, 0, 0);
    $new_node->field_entity_reference[LANGUAGE_NONE][0]['target_id'] = $form_state['values']['nid'];
    $new_node->body[LANGUAGE_NONE][0]['value'] = $form_state['values']['title'];
    node_save($new_node);
    drupal_set_message(t('Note was saved'), 'status');
    drupal_goto();
}

function rm_sales_useraccounts_form($form, &$form_state) {

    drupal_add_js(drupal_get_path('module', 'rm_sales') . '/js/rm_sales.js');
    
    $form = array();
    
    $form['filter']['#prefix'] = '<br /><div class="input-group"> <span class="input-group-addon">Filter</span><input id="filterSuggestions" type="text" class="form-control" placeholder="Angezeigte Benutzerkonten filtern"></div><br />';
    
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
		// 'field_address_locality' => array(
			// 'data' => t('City'),
			// 'type' => 'field',
			// 'specifier' => array(
				// 'field' => 'field_address',
				// 'column' => 'locality',
			// ),
		// ),
		// 'field_address_zip' => array(
			// 'data' => t('Zip'),
			// 'type' => 'field',
			// 'specifier' => array(
				// 'field' => 'field_address',
				// 'column' => 'postal_code',
			// ),
		// ),
		// 'field_address_street' => array(
			// 'data' => t('Street'),
			// 'type' => 'field',
			// 'specifier' => array(
				// 'field' => 'field_address',
				// 'column' => 'thoroughfare',
			// ),
		// ),
        'actions' => t('Actions'),
	);
    
    
        $query = new EntityFieldQuery();
        $query
            ->entityCondition('entity_type', 'node')
            ->entityCondition('bundle', array('seller_profile', 'customer_profile'));
        if(!rm_user_is_admin()) $query->fieldCondition('field_salesguy', 'target_id', $user->uid);
        $tmp = $query
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
    
    if(!empty($alluids)) {
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
    }
    if(!empty($tmp['user'])) {
        $alluseraccounts = entity_load('user', array_keys($tmp['user']));
        
        foreach($alluseraccounts as $uid => $userobject) {
        
            if($userobject->status) {
                $statusswitch = l(t('block'), 'user/' . $user->uid . '/disable/' . $uid, array('attributes' => array('class' => array('btn', 'btn-sm', 'btn-danger')), 'query' => drupal_get_destination()));
            }
            else {
                $statusswitch = l(t('unblock'), 'user/' . $user->uid . '/enable/' . $uid, array('attributes' => array('class' => array('btn', 'btn-sm', 'btn-warning')), 'query' => drupal_get_destination()));
            }
            
            $field_address_locality = (!empty($userobject->field_address[LANGUAGE_NONE][0]['locality'])) ? $userobject->field_address[LANGUAGE_NONE][0]['locality'] : '';
            $field_address_zip = (!empty($userobject->field_address[LANGUAGE_NONE][0]['postal_code'])) ? $userobject->field_address[LANGUAGE_NONE][0]['postal_code'] : '';
            $field_address_street = (!empty($userobject->field_address[LANGUAGE_NONE][0]['thoroughfare'])) ? $userobject->field_address[LANGUAGE_NONE][0]['thoroughfare'] : '';

			$options[$uid] = array(
				'status' => ($userobject->status) ? t('Active') : t('Blocked'),
				'uid' => $uid,
                'field_gender' => (!empty($userobject->field_gender[LANGUAGE_NONE][0]['value'])) ? $userobject->field_gender[LANGUAGE_NONE][0]['value'] : '',
				'name' => format_username($userobject) . '<br>' . $field_address_street . '<br>' . $field_address_zip . ' ' . $field_address_locality,
                
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

function rm_sales_profilecare_form() {

    drupal_add_js(drupal_get_path('module', 'rm_sales') . '/js/rm_sales.js');

	global $user;
	
	$header = array(
		'created' => array(
			'data' => t('Created'),
			'type' => 'property',
			'specifier' => 'created',
			'sort' => 'desc',
		),
		'title' => array(
			'data' => t('Name'),
			'type' => 'property',
			'specifier' => 'title',
			'sort' => 'asc',
		),
        'field_next_action' => array(
			'data' => t('Next action'),
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_next_action',
				'column' => 'value',
			),
		),
        'field_offerinfo_url' => array(
			'data' => t('Offer URL'),
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_offerinfo_url',
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
			'data' => 'Prio',
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_priority',
				'column' => 'value',
			),
		),
        'support_status' => t('Support'),
        'action_buttons' => t('Actions'),
	);
	
    $usergroup1 = rm_user_get_users_by_role('salesguy');
    $usergroup2 = rm_user_get_users_by_role('admin');
    $usergroup = array_merge($usergroup1, $usergroup2);
    $usergrouploaded = entity_load('user', $usergroup);
    $excludable = array();
    $allbutme = array();
    foreach($usergrouploaded as $usergroupobject) {
        $excludable[] = $usergroupobject->mail;
        if($usergroupobject->mail != $user->mail) $allbutme[] = $usergroupobject->uid;
    }
        
    $queryparams = drupal_get_query_parameters();
    
	$query = new EntityFieldQuery();
	$query
		->entityCondition('entity_type', 'node')
		->entityCondition('bundle', array('seller_profile', 'customer_profile'));
    
    //Filter injections
    if(!empty($queryparams['sales_step'])) {
        switch($queryparams['sales_step']) {
            case 'prospect':
                $query->propertyCondition('prospect', 1);
                $query->propertyCondition('suppressed', 0);
                $query->propertyCondition('status', 0);
                break;
            case 'registered':
                $query->propertyCondition('status', 1);
                break;
            case 'lead':
                $query->propertyCondition('prospect', 0);
                $query->propertyCondition('suppressed', 0);
                $query->propertyCondition('status', 0);
                break;
            case 'suppressed':
                $query->propertyCondition('suppressed', 1);
                break;
            default:
                $query->propertyCondition('suppressed', 0);
                break;
        }
		
	}
    
    if(!empty($queryparams['priority']) && $queryparams['priority'] > 0) {
        $query->fieldCondition('field_priority', 'value', $queryparams['priority']);
	}
    
    if(!empty($queryparams['profiletype']) && $queryparams['profiletype'] != 'all') {
        $query->propertyCondition('type', $queryparams['profiletype']);
	}
    
    if(!empty($queryparams['salesguy'])) {
        if($queryparams['salesguy'] > 0) {
            $query->fieldCondition('field_salesguy', 'target_id', $queryparams['salesguy']);
        }
        else {
            switch($queryparams['salesguy']) {
                case '-3':
                    $query->propertyCondition('sticky', 1);
                    break;
                case '-2':
                    $query->propertyCondition('sticky', 0);
                    break;
            }
        }
    }
    
    if(!empty($queryparams['title'])) {
        $query->propertyCondition('title', $queryparams['title'], 'CONTAINS');
	}
    
    if(!empty($queryparams['source_url'])) {
        $query->fieldCondition('field_source_url', 'value', $queryparams['source_url'], 'CONTAINS');
	}
    
    if(!empty($queryparams['street'])) {
        $query->fieldCondition('field_address', 'thoroughfare', $queryparams['street'], 'CONTAINS');
	}
    
    if(!empty($queryparams['zip'])) {
        $query->fieldCondition('field_address', 'postal_code', $queryparams['zip'], 'CONTAINS');
	}
    
    if(!empty($queryparams['locality'])) {
        $query->fieldCondition('field_address', 'locality', $queryparams['locality'], 'CONTAINS');
	}
    
	$tmp = $query
		// ->tableSort($header)
		// ->pager(100)
		->execute();
    
    $allkeys_1 = array();
    if(isset($tmp['node'])) {
        $allkeys_1 = array_keys($tmp['node']);
    }
    
    $allkeys_2 = array();
    if(!rm_user_is_admin()) {
        $query2 = new EntityFieldQuery();
        $tmp = $query2
            ->entityCondition('entity_type', 'node')
            ->entityCondition('bundle', array('seller_profile', 'customer_profile'))
            ->fieldCondition('field_salesguy', 'target_id', array($allbutme))
            ->execute();
        if(isset($tmp['node'])) {
            $allkeys_2 = array_keys($tmp['node']);
        }
    }
    
    $resultingnids = array_diff($allkeys_1, $allkeys_2);
    
    $tmp = array();
    if(!empty($resultingnids)) {
        $query3 = new EntityFieldQuery();
        $tmp = $query3
            ->entityCondition('entity_type', 'node')
            ->entityCondition('bundle', array('seller_profile', 'customer_profile'))
            ->propertyCondition('nid', $resultingnids)
            ->tableSort($header)
            ->pager(100)
            ->execute();
    }
	$filterform = drupal_get_form('rm_sales_profilecare_filter_form');
		
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
			
            
            if($profileobject->type == 'seller_profile') {
                $shipping_agreement = '';
                if($profileobject->status) $shipping_agreement = '<p class="text-center"><span class="label label-danger"><i class="fa fa-ban"></i></span></p>';
                $agrnodes = NULL;
                if(isset($profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id'])) $agrnodes = rm_api_get_nodes_by_properties(array('shipping_agreement'), 1, -1, -1, -1, -1, $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id']);
                if(isset($agrnodes)) {
                    $shipping_agreement = '<p class="text-center"><span class="label label-success"><i class="fa fa-check"></i></span></p>';
                }
                
                $dispatch_agreement = '';
                if($profileobject->status) $dispatch_agreement = '<p class="text-center"><span class="label label-danger"><i class="fa fa-ban"></i></span></p>';
                $agrnodes = NULL;
                if(isset($profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id'])) $agrnodes = rm_api_get_nodes_by_properties(array('dispatch_agreement'), 1, -1, -1, -1, -1, $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id']);
                if(isset($agrnodes)) {
                    $dispatch_agreement = '<p class="text-center"><span class="label label-success"><i class="fa fa-check"></i></span></p>';
                }
                
                $pickup_agreement = '';
                if($profileobject->status) $pickup_agreement = '<p class="text-center"><span class="label label-danger"><i class="fa fa-ban"></i></span></p>';
                $agrnodes = NULL;
                if(isset($profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id'])) $agrnodes = rm_api_get_nodes_by_properties(array('pickup_agreement'), 1, -1, -1, -1, -1, $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id']);
                if(isset($agrnodes)) {
                    $pickup_agreement = '<p class="text-center"><span class="label label-success"><i class="fa fa-check"></i></span></p>';
                }
                
                $delivery_agreement = '';
                if($profileobject->status) $delivery_agreement = '<span class="label label-danger"><i class="fa fa-truck"></i> </span>';
                if((strpos($pickup_agreement, 'label-success') !== false) || (strpos($dispatch_agreement, 'label-success') !== false) || (strpos($shipping_agreement, 'label-success') !== false)) $delivery_agreement = '<span class="label label-success"><i class="fa fa-truck"></i> </span>';
                
                $commission_agreement = '';
                if($profileobject->status) $commission_agreement = '<span class="label label-danger">% ' . l(t('add new one'), 'manage/seller/addagreement/commission_agreement/' . $profileobject->uid, array('query' => drupal_get_destination(), 'html' => TRUE)) . '</span>';
                $agrnodes = NULL;
                if(isset($profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id'])) $agrnodes = rm_api_get_nodes_by_properties(array('commission_agreement'), 1, -1, -1, -1, -1, $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id']);
                if(isset($agrnodes)) {
                    $commission_agreement = '';
                    //$commission_agreement = l(t('add new one'), 'manage/seller/addagreement/commission_agreement/' . $profileobject->uid, array('query' => drupal_get_destination(), 'html' => TRUE, 'attributes' => array('class' => array('btn', 'btn-sm', 'btn-success'))));
                    foreach($agrnodes as $agrnid => $agrnode) {
                        $agrtitle = date('d.m.Y', $agrnode->changed) . ' ';
                        if(!empty($agrnode->field_user_reference[LANGUAGE_NONE][0]['target_id'])) {
                            $userobject = user_load($agrnode->field_user_reference[LANGUAGE_NONE][0]['target_id']);
                            $agrtitle .= t('for @user', array('@user' => format_username($userobject)));
                        }
                        else {
                            $agrtitle .= t('for all');
                        }
                        $commission_agreement .= '<span class="label label-success">% ' . l($agrtitle, 'manage/seller/editagreement/' . $agrnode->nid, array('html' => TRUE, 'query' => drupal_get_destination(), 'attributes' => array('class' => array('')))) . '</span>';
                    }
                    $commission_agreement .= ' <span class="small">' . l(t('add new one'), 'manage/seller/addagreement/commission_agreement/' . $profileobject->uid, array('query' => drupal_get_destination(), 'html' => TRUE)) . '</span>';
                }
                
                $payment_agreement = '';
                if($profileobject->status) $payment_agreement = '<span class="label label-danger"><i class="fa fa-money"></i></span>';
                $agrnodes = NULL;
                if(isset($profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id'])) $agrnodes = rm_api_get_nodes_by_properties(array('payment_agreement'), 1, -1, -1, -1, -1, $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id']);
                if(isset($agrnodes)) {
                    $payment_agreement = '<span class="label label-success"><i class="fa fa-money"></i></span>';
                }
                
                $offer_table = '';
                if($profileobject->status) $offer_table = '<span class="label label-danger"><i class="fa fa-table"></i> ' . l(t('add new one'), 'manage/sales/addoffertable/' . $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id'], array('query' => drupal_get_destination(), 'html' => TRUE)) . '</span>';;
                $agrqry = new EntityFieldQuery();
                $agrtmp = array();
                if(isset($profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id'])) {
                    $agrtmp = $agrqry
                        ->entityCondition('entity_type', 'node')
                        ->entityCondition('bundle', 'offer_table')
                        ->propertyCondition('status', 1)
                        ->propertyCondition('uid', $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id'])
                        ->propertyOrderBy('created', 'DESC')
                        ->range(0, 1)
                        ->execute();
                }
                
                if(isset($agrtmp['node'])) {
                    $agrtmpkeys = array_keys($agrtmp['node']);
                    $offertable = node_load($agrtmpkeys[0]);
                    $offer_table = '<span class="label label-success"><i class="fa fa-table"></i> ' . l('Angebotsdaten', file_create_url($offertable->field_offer_table[LANGUAGE_NONE][0]['uri']), array('html' => TRUE)) . '</span> <span class="small">' . l(t('add new one'), 'manage/sales/addoffertable/' . $profileobject->field_user_reference[LANGUAGE_NONE][0]['target_id'], array('query' => drupal_get_destination(), 'html' => TRUE)) . '</span>';
                }
                
                $all_offers = '';
                if($profileobject->status) {
                    $offers = rm_shop_get_structured_seller_offers($profileobject->uid, array(0,1));
                    $offercount = count($offers);
                    $inactive = 0;
                    foreach($offers as $offer) {
                        if($offer->status == 0) $inactive++;
                    }
                    $active = $offercount - $inactive;
                    if($offercount > 0 && $offercount == $active) {
                        $color = 'success';
                    }
                    elseif($offercount > 0 && $offercount > $active && $active > 0) {
                        $color = 'warning';
                    }
                    else {
                        $color = 'danger';
                    }
                    $all_offers = '<span class="label label-' . $color . '"><i class="fa fa-shopping-cart"></i> ' . $active . '/' . $offercount . '</span>';
                    
                }
                
                $offerinfourl = '';
                if(isset($profileobject->field_offerinfo_url[LANGUAGE_NONE][0]['value'])) $offerinfourl .= $profileobject->field_offerinfo_url[LANGUAGE_NONE][0]['value'];
			}
			
            $suggestion_count = '';
            if($profileobject->status == 1) {
            
                $sugqry = new EntityFieldQuery();
                $profileowner = user_load($profileobject->uid);
                $sugcount = $sugqry
                    ->entityCondition('entity_type', 'node')
                    ->entityCondition('bundle', array('customer_profile', 'seller_profile'))
                    ->fieldCondition('field_suggester', 'value', $profileowner->mail)
                    ->count()
                    ->execute();
                
                if($sugcount == 0) {
                    $color = 'danger';
                }
                else if($sugcount < 5) {
                    $color = 'warning';
                }
                else {
                    $color = 'success';
                }
                $suggestion_count = '<span class="label label-' . $color . '">' . $sugcount . ' (' . $profileowner->mail . ')</span>';
            }
            
              
            $support_status = '<ul class="list-unstyled">';
            if(!empty($all_offers) && $profileobject->type == 'seller_profile') $support_status .= '<li>' . $all_offers . '</li>';
            if(!empty($delivery_agreement) && $profileobject->type == 'seller_profile') $support_status .= '<li>' . $delivery_agreement . '</li>';
            if(!empty($payment_agreement) && $profileobject->type == 'seller_profile') $support_status .= '<li>' . $payment_agreement . '</li>';
            if(!empty($suggestion_count)) $support_status .= '<li>' . $suggestion_count . '</li>';
            if(!empty($commission_agreement) && $profileobject->type == 'seller_profile') $support_status .= '<li>' . $commission_agreement . '</li>';
            if(!empty($offer_table) && $profileobject->type == 'seller_profile') $support_status .= '<li>' . $offer_table . '</li>';
            $support_status .= '</ul>';
            
            $salesguynid = '';
            if(rm_user_is_admin($user->uid)) {
                $salesguynid = '<div class="salesguys"><select id="salesguy_' . $nid . '" class="form-control input-sm">';
                $salesguys = entity_load('user', rm_user_get_users_by_role('salesguy'));
                foreach($salesguys as $uid => $userobject) {
                    $salesguynid .= '<option value="' . $uid . '"';
                    if(isset($profileobject->field_salesguy[LANGUAGE_NONE][0]['target_id']) && $profileobject->field_salesguy[LANGUAGE_NONE][0]['target_id'] == $uid) $salesguynid .= ' selected';
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
                    $notes .= '</span>' . $noteobject->body[LANGUAGE_NONE][0]['value'] . '</li>';
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
            if(!empty($profileobject->field_next_action[LANGUAGE_NONE][0]['value']) && $profileobject->field_next_action[LANGUAGE_NONE][0]['value'] < 1924988399) $nextaction = substr(date('c', $profileobject->field_next_action[LANGUAGE_NONE][0]['value']), 0, 16);
            
            $publishingpossiblestring = '';
			$publishingpossible = rm_api_node_validate_required($profileobject->nid);
			if(empty($publishingpossible) && $profileobject->status == 0) $publishingpossiblestring = '<br>' . l(t('Publish'), 'manage/sales/' . $nid . '/publish', array('query' => drupal_get_destination(), 'attributes' => array('class' => array('btn', 'btn-sm', 'btn-success'))));
            
            $deletepossiblestring = '';
            if($profileobject->status == 0) $deletepossiblestring = l(t('Delete'), 'manage/sales/' . $nid . '/delete', array('query' => drupal_get_destination(), 'attributes' => array('class' => array('btn', 'btn-sm', 'btn-danger'))));
            
            $shadowpossiblestring = '';
            if($profileobject->uid > 0) $shadowpossiblestring = '<br>' . l(t('Shadow log in'), 'user/' . $user->uid . '/shadow/' . $profileobject->uid, array('attributes' => array('class' => array('btn', 'btn-sm', 'btn-info'))));
            
            switch($profileobject->type) {
				case "customer_profile": $nodetypename = 'seller_profile'; break;
				case "seller_profile": $nodetypename = 'customer_profile'; break;
			}
            
            $suggestedcount = 0;
            $suggestedby = '';
            if(isset($profileobject->field_suggester[LANGUAGE_NONE])) {
                foreach($profileobject->field_suggester[LANGUAGE_NONE] as $suggestermail) {
                    if(!in_array($suggestermail['value'], $excludable)) {
                        $suggestedcount++;
                        $suggestedby .= $suggestermail['value'] . ' ';
                    }
                }
            }
            
            $suggestedcount = ($suggestedcount) ? '<span class="label label-success suggester-tooltip" data-toggle="tooltip" data-placement="top" title="' . $suggestedby . '">' . $suggestedcount . '</span>' : '<span class="label label-danger suggester-tooltip" data-toggle="tooltip" data-placement="top" title="von Regiomino vorgeschlagen">0</span>';
            
            $addsuggesterlink = '<br>(' . l(t('More suggesters') . '?', 'manage/addsuggesters/' . $profileobject->nid, array('query' => drupal_get_destination())) . ')';
            
            if($profileobject->status) {
                if(!rm_user_gtc_accepted($profileobject->nid)) {
                    if(empty($profileobject->field_register_hash[LANGUAGE_NONE][0]['value'])) {
                        $invitelabel = t('Send invite');
                        $invitebtntype = 'btn-success';
                    }
                    else {
                        $invitelabel = t('Send invite again');
                        $invitebtntype = 'btn-warning';
                    }
                    $addsuggesterlink .= '<br>' . l($invitelabel, 'manage/sales/sendinvite/' . $nid, array('query' => drupal_get_destination(), 'attributes' => array('class' => array('btn', 'btn-sm', $invitebtntype))));
                }
            }
            
            if($profileobject->sticky) {
                $action_buttons = l(t('edit'), 'manage/sales/' . $nid . '/edit', array('query' => drupal_get_destination(), 'attributes' => array('class' => array('btn', 'btn-sm', 'btn-warning')))) . l(t('Add note'), 'manage/sales/addnote/' . $nid, array('query' => drupal_get_destination(), 'attributes' => array('class' => array('btn', 'btn-sm', 'btn-primary')))) . $deletepossiblestring . l(($profileobject->prospect) ? t('Remove prospect') : t('Make prospect'), ($profileobject->prospect) ? 'manage/sales/removeprospect/' . $nid : 'manage/sales/makeprospect/' . $nid, array('query' => drupal_get_destination(), 'attributes' => array('class' => array('btn', 'btn-sm', 'btn-info')))) . l(($profileobject->suppressed) ? t('Unsuppress') : t('Suppress'), ($profileobject->suppressed) ? 'manage/sales/unsuppress/' . $nid : 'manage/sales/suppress/' . $nid, array('query' => drupal_get_destination(), 'attributes' => array('class' => array('btn', 'btn-sm', 'btn-info')))) . l(t('Change to "@nodetype"', array('@nodetype' => node_type_get_name($nodetypename))), 'manage/sales/changeprofiletype/' . $nid . '/' . $nodetypename, array('query' => drupal_get_destination(), 'attributes' => array('class' => array('btn', 'btn-sm', 'btn-default')))) . $salesguynid;
            }
            else {
                $action_buttons = l(t('Take over'), 'manage/sales/assignprofile/' . $nid . '/' . $user->uid, array('query' => drupal_get_destination(), 'attributes' => array('class' => array('btn', 'btn-sm', 'btn-success'))));
            }
			$profiletitle = ($profileobject->status) ? l($profileobject->title, 'node/' . $profileobject->nid) : $profileobject->title;
            
            $source_url_string = (!empty($profileobject->field_source_url[LANGUAGE_NONE][0]['value'])) ? $profileobject->field_source_url[LANGUAGE_NONE][0]['value'] : '';
			$options[$nid] = array(
				'created' => date('d.m.Y H:i', $profileobject->created) . $addsuggesterlink,
				'title' => $profiletitle . $publishingpossiblestring . $shadowpossiblestring . '<br>' . $street . '<br>' . $zip . ' ' . $locality . '<br>' . $publicphone . '<br />Vorgeschlagen von: ' . $suggestedcount,
                'field_next_action' => '<div class="nextactions"><input id="nextaction_' . $nid . '" class="form-control" type="datetime-local" value="' . $nextaction . '" /></div>',
                'field_offerinfo_url' => ($profileobject->type == 'seller_profile') ? '<div class="offerinfourl"><input type="text" id="offerinfourl_' . $nid . '" class="form-control" value="' . $offerinfourl . '"></div>' : '',
                'field_salescomment' => '<small>Quelle: ' . $source_url_string . '</small><br><br>' . $salescomment . '<br>' . $notes,
                'field_priority' => $prionid,
				// 'field_address_locality' => $locality,
				// 'field_address_zip' => $zip,
				// 'field_address_street' => $street,
				// 'field_publicphone' => $publicphone,
                // 'field_salesguy' => $salesguynid,
				// 'shipping_agreement' => $shipping_agreement,
				// 'dispatch_agreement' => $dispatch_agreement,
				// 'pickup_agreement' => $pickup_agreement,
                'support_status' => $support_status,
                // 'all_offers' => $all_offers,
                // 'delivery_agreement' => $delivery_agreement,
				// 'commission_agreement' => $commission_agreement,
				// 'payment_agreement' => $payment_agreement,
                // 'suggestions' => $suggestion_count,
				// 'offer_table' => $offer_table,
                'action_buttons' => $action_buttons,
			);
            
            
		}
		
		// $form['submit'] = array(
			// '#type' => 'submit',
			// '#value' => t('Export to XLS'),
			// '#submit' => array('rm_sales_export_profiles'),
		// );
		
		// $form['suggestions'] = array(
			// '#type' => 'tableselect',
			// '#header' => $header,
			// '#options' => $options,
			// '#attributes' => array(),
		// );
		
		// $form['#header'] = $header;
		
		// $form['pager'] = array('#markup' => theme('pager'));
        
        $output = theme('table', array('header' => $header, 'rows' => $options, 'attributes' => array('class' => array('table'))));
	}
	else {
        $output = '';
		drupal_set_message(t('Currently there are no active profiles under your supervision. You should change that...'), 'warning');
	}
    
    $filter = '<div class="row"><div class="input-group"> <span class="input-group-addon">Einschränken</span><input id="filterSuggestions" type="text" class="form-control" placeholder="Angezeigte Ergebnisse weiter einschränken"></div></div>';
    
    $exportall = '<div class="row pull-right">' . l(t('Export all filtered'), 'manage/sales/manage/export', array('attributes' => array('class' => array('btn', 'btn-success', 'btn-sm')), 'query' => drupal_get_query_parameters())) . '</div>';
    
    

	return $exportall . render($filterform) . '<br /><div class="row">' . $output . theme('pager') . '</div>';
}

function rm_sales_addsuggesters_form($form, &$form_state, $nid) {
    $form = array();
    
    $form['suggester'] = array(
        '#type' => 'fieldset',
    );
    
    $form['suggester']['nid'] = array(
        '#type' => 'value',
        '#value' => $nid,
    );
    
    $profileobject = node_load($nid);
    
    rm_api_attach_field_to_form('field_suggester', 'node', $profileobject->type, $profileobject, $form['suggester'], $form_state, LANGUAGE_NONE);
    
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
    );
    
    return $form;
}

function rm_sales_addsuggesters_form_submit($form, &$form_state) {
    $profileobject = node_load($form_state['values']['nid']);
    foreach($form_state['values']['field_suggester'][LANGUAGE_NONE] as $key => $value) {
        if(!is_numeric($key) || empty($value['value'])) unset($form_state['values']['field_suggester'][LANGUAGE_NONE][$key]);
    }
    $profileobject->field_suggester = $form_state['values']['field_suggester'];
    if(!empty($profileobject->field_suggester[LANGUAGE_NONE])) {
        $profileobject->field_suggester[LANGUAGE_NONE] = array_values($profileobject->field_suggester[LANGUAGE_NONE]);
    }
    else {
        unset($profileobject->field_suggester[LANGUAGE_NONE]);
    }
    node_save($profileobject);
    global $user;
	rm_user_activity(
		t('Added suggesters to <a href="@url">@profile</a>', array('@url' => url('node/' . $profileobject->nid), '@profile' => $profileobject->title)),
		$profileobject->nid,
		'node',
		$user->uid
	);
}

/* function rm_sales_profileset_form() {

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
		// 'field_address_locality' => array(
			// 'data' => t('City'),
			// 'type' => 'field',
			// 'specifier' => array(
				// 'field' => 'field_address',
				// 'column' => 'locality',
			// ),
		// ),
		// 'field_address_zip' => array(
			// 'data' => t('Zip'),
			// 'type' => 'field',
			// 'specifier' => array(
				// 'field' => 'field_address',
				// 'column' => 'postal_code',
			// ),
		// ),
		// 'field_address_street' => array(
			// 'data' => t('Street'),
			// 'type' => 'field',
			// 'specifier' => array(
				// 'field' => 'field_address',
				// 'column' => 'thoroughfare',
			// ),
		// ),
		// 'field_publicphone' => array(
			// 'data' => t('Phone'),
			// 'type' => 'field',
			// 'specifier' => array(
				// 'field' => 'field_publicphone',
				// 'column' => 'number',
			// ),
		// ),
		'field_next_action' => array(
			'data' => t('Next action'),
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
    // $query
		// ->tableSort($header);
		// ->pager(100);
    $tmp = $query->execute();
		
	$form = array();
    
    //$form['filter']['#prefix'] = '<br /><div class="input-group"> <span class="input-group-addon">Filter</span><input id="filterSuggestions" type="text" class="form-control" placeholder="Angezeigte Profile filtern"></div><br />';
	
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
            if(!empty($profileobject->field_next_action[LANGUAGE_NONE][0]['value']) && $profileobject->field_next_action[LANGUAGE_NONE][0]['value'] < 1924988399) $nextaction = date('Y-m-d', $profileobject->field_next_action[LANGUAGE_NONE][0]['value']);
            
			$options[$nid] = array(
                'prospect' => ($profileobject->prospect) ? t('Yes') : t('No'),
                //'suggester' => (!empty($profileobject->field_suggester[LANGUAGE_NONE])) ? count($profileobject->field_suggester[LANGUAGE_NONE]) : 0,
				'type' => node_type_get_name($profileobject),
                'title' => $profileobject->title . '<br>' . $street . '<br>' . $zip . ' ' . $locality . '<br>' . $publicphone,
				// 'field_address_locality' => $locality,
				// 'field_address_zip' => $zip,
				// 'field_address_street' => $street,
				// 'field_publicphone' => $publicphone,
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
} */

function rm_sales_export_profiles() {
	$loadarray = array();
    global $user;
	if(empty($loadarray)) {
    
        $usergroup1 = rm_user_get_users_by_role('salesguy');
        $usergroup2 = rm_user_get_users_by_role('admin');
        $usergroup = array_merge($usergroup1, $usergroup2);
        $usergrouploaded = entity_load('user', $usergroup);
        $excludable = array();
        $allbutme = array();
        foreach($usergrouploaded as $usergroupobject) {
            $excludable[] = $usergroupobject->mail;
            if($usergroupobject->mail != $user->mail) $allbutme[] = $usergroupobject->uid;
        }
    
        $queryparams = drupal_get_query_parameters();
    
    
        $query = new EntityFieldQuery();
        $query
            ->entityCondition('entity_type', 'node')
            ->entityCondition('bundle', array('seller_profile', 'customer_profile'));
        
        //Filter injections
        if(!empty($queryparams['sales_step'])) {
            switch($queryparams['sales_step']) {
                case 'prospect':
                    $query->propertyCondition('prospect', 1);
                    $query->propertyCondition('suppressed', 0);
                    $query->propertyCondition('status', 0);
                    break;
                case 'registered':
                    $query->propertyCondition('status', 1);
                    break;
                case 'lead':
                    $query->propertyCondition('prospect', 0);
                    $query->propertyCondition('suppressed', 0);
                    $query->propertyCondition('status', 0);
                    break;
                case 'suppressed':
                    $query->propertyCondition('suppressed', 1);
                    break;
                default:
                    $query->propertyCondition('suppressed', 0);
                    break;
            }
            
        }
        
        if(!empty($queryparams['priority']) && $queryparams['priority'] > 0) {
            $query->fieldCondition('field_priority', 'value', $queryparams['priority']);
        }
        
        if(!empty($queryparams['profiletype']) && $queryparams['profiletype'] != 'all') {
            $query->propertyCondition('type', $queryparams['profiletype']);
        }
        
        if(!empty($queryparams['salesguy'])) {
            if($queryparams['salesguy'] > 0) {
                $query->fieldCondition('field_salesguy', 'target_id', $queryparams['salesguy']);
            }
            else {
                switch($queryparams['salesguy']) {
                    case '-3':
                        $query->propertyCondition('sticky', 1);
                        break;
                    case '-2':
                        $query->propertyCondition('sticky', 0);
                        break;
                }
            }
        }
        
        if(!empty($queryparams['title'])) {
            $query->propertyCondition('title', $queryparams['title'], 'CONTAINS');
        }
        
        if(!empty($queryparams['street'])) {
            $query->fieldCondition('field_address', 'thoroughfare', $queryparams['street'], 'CONTAINS');
        }
        
        if(!empty($queryparams['zip'])) {
            $query->fieldCondition('field_address', 'postal_code', $queryparams['zip'], 'CONTAINS');
        }
        
        if(!empty($queryparams['locality'])) {
            $query->fieldCondition('field_address', 'locality', $queryparams['locality'], 'CONTAINS');
        }
        
        $tmp = $query
            // ->tableSort($header)
            // ->pager(100)
            ->execute();
        
        $allkeys_1 = array();
        if(isset($tmp['node'])) {
            $allkeys_1 = array_keys($tmp['node']);
        }
        
        $allkeys_2 = array();
        if(!rm_user_is_admin()) {
            $query2 = new EntityFieldQuery();
            $tmp = $query2
                ->entityCondition('entity_type', 'node')
                ->entityCondition('bundle', array('seller_profile', 'customer_profile'))
                ->fieldCondition('field_salesguy', 'target_id', array($allbutme))
                ->execute();
            if(isset($tmp['node'])) {
                $allkeys_2 = array_keys($tmp['node']);
            }
        }
        
        $resultingnids = array_diff($allkeys_1, $allkeys_2);
        
        $tmp = array();
        if(!empty($resultingnids)) {
            $query3 = new EntityFieldQuery();
            $tmp = $query3
                ->entityCondition('entity_type', 'node')
                ->entityCondition('bundle', array('seller_profile', 'customer_profile'))
                ->propertyCondition('nid', $resultingnids)
                ->execute();
        }
    
		$loadarray = array_keys($tmp['node']);
	}
	$allprofiles = entity_load('node', $loadarray);
	$header = array(
		t('Profiletype'),
        t('Status'),
        t('Completed profile'),
		t('Name'),
        t('Source URL'),
		t('E-Mail'),
		t('Street'),
		t('Zip'),
		t('Locality'),
		t('Phone'),
		t('Salesguy'),
        t('Suggested by'),
        t('Suggestions'),
		t('Shipping agreement'),
		t('Dispatch agreement'),
		t('Pickup agreement'),
		t('Commission agreement'),
		t('Payment agreement'),
		t('Offer table'),
        t('Comments'),
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
        
        $shipnodes = '';
        $disnodes = '';
        $picknodes = '';
        $commnodes = '';
        $paynodes = '';
        $ofrtbl = '';
        $sugcount = 0;
        if($profileobject->status == 1) {
            $sugqry = new EntityFieldQuery();
            $profileowner = user_load($profileobject->uid);
            $sugcount = $sugqry
                ->entityCondition('entity_type', 'node')
                ->entityCondition('bundle', array('customer_profile', 'seller_profile'))
                ->fieldCondition('field_suggester', 'value', $profileowner->mail)
                ->count()
                ->execute();
        }
        $suggestedby = '';
        if(isset($profileobject->field_suggester[LANGUAGE_NONE])) {
            foreach($profileobject->field_suggester[LANGUAGE_NONE] as $suggestermail) {
                if(!in_array($suggestermail['value'], $excludable)) {
                    $suggestedby .= $suggestermail['value'] . ' ';
                }
            }
        }
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
        
        
        if($profileobject->suppressed) {
            $currentstatus = 'Sleep';
        }
        else if(rm_user_gtc_accepted($profileobject->nid)) {
            $currentstatus = 'AGB akzeptiert';
        }
        else if(!empty($profileobject->field_register_hash[LANGUAGE_NONE][0]['value'])) {
            $currentstatus = 'Eingeladen';
        }
        else if($profileobject->status) {
            $currentstatus = 'Veröffentlicht';
        }
        else if($profileobject->prospect) {
            $currentstatus = 'Prospect';
        }
        else {
            $currentstatus = 'Lead';
        }
        
        
		$rows[] = array(
			node_type_get_name($profileobject->type),
            $currentstatus,
            (empty($valid)) ? t('Yes') : t('No'),
			$profileobject->title,
            (isset($profileobject->field_source_url[LANGUAGE_NONE][0]['value'])) ? $profileobject->field_source_url[LANGUAGE_NONE][0]['value'] : '',
            (isset($profileobject->field_email[LANGUAGE_NONE][0]['email'])) ? $profileobject->field_email[LANGUAGE_NONE][0]['email'] : '',
			(isset($profileobject->field_address[LANGUAGE_NONE][0]['thoroughfare'])) ? $profileobject->field_address[LANGUAGE_NONE][0]['thoroughfare'] : '',
			$zip,
			(isset($profileobject->field_address[LANGUAGE_NONE][0]['locality'])) ? $profileobject->field_address[LANGUAGE_NONE][0]['locality'] : '',
			$phone,
            $username,
            $suggestedby,
            $sugcount,
            (isset($shipnodes) && !empty($shipnodes)) ? t('Yes') : t('No'),
            (isset($disnodes) && !empty($disnodes)) ? t('Yes') : t('No'),
            (isset($picknodes) && !empty($picknodes)) ? t('Yes') : t('No'),
            (isset($commnodes) && !empty($commnodes)) ? t('Yes') : t('No'),
            (isset($paynodes) && !empty($paynodes)) ? t('Yes') : t('No'),
            (isset($ofrtbl) && !empty($ofrtbl)) ? t('Yes') : t('No'),
            (isset($profileobject->field_salescomment[LANGUAGE_NONE][0]['value'])) ? $profileobject->field_salescomment[LANGUAGE_NONE][0]['value'] : '',
            $notes,
		);
	}
	rm_api_excel_export($header, $rows);
}


function rm_sales_profilequeue_form() {

    drupal_add_js(drupal_get_path('module', 'rm_sales') . '/js/rm_sales.js');

	global $user;
	
	$header = array(
        'created' => array(
            'data' => t('Created'),
            'type' => 'property',
            'specifier' => 'created',
            'sort' => 'desc',
        ),
        'prospect' => array(
            'data' => t('Prospect'),
            'type' => 'property',
            'specifier' => 'prospect',
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
		//->pager(100)
		->execute();
		
	$form = array();
    
    $form['filter']['#prefix'] = '<br /><div class="input-group"> <span class="input-group-addon">Filter</span><input id="filterSuggestions" type="text" class="form-control" placeholder="Angezeigte Profile filtern"></div><br />';
	
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
                'created' => date('d.m.Y H:i', $profileobject->created),
				'prospect' => ($profileobject->prospect) ? t('Yes') : t('No'),
                'type' => node_type_get_name($profileobject),
				'title' => $profileobject->title . '<br>' . $street . '<br>' . $zip . ' ' . $locality . '<br>' . $publicphone,
				/* 'field_address_locality' => $locality,
				'field_address_zip' => $zip,
				'field_address_street' => $street,
				'field_publicphone' => $publicphone, */
				'actions' => l(t('Take over'), 'manage/sales/assignprofile/' . $nid . '/' . $user->uid, array('query' => drupal_get_destination(), 'attributes' => array('class' => array('btn', 'btn-sm', 'btn-success')))) . ' ' . l(t('Delete'), 'manage/sales/' . $nid . '/delete', array('query' => drupal_get_destination(), 'attributes' => array('class' => array('btn', 'btn-sm', 'btn-danger')))) . ' ' . l(($profileobject->prospect) ? t('Remove prospect') : t('Make prospect'), ($profileobject->prospect) ? 'manage/sales/removeprospect/' . $nid : 'manage/sales/makeprospect/' . $nid, array('query' => drupal_get_destination(), 'attributes' => array('class' => array('btn', 'btn-sm', 'btn-info')))) . ' ' . l(($profileobject->suppressed) ? t('Unsuppress') : t('Suppress'), ($profileobject->suppressed) ? 'manage/sales/unsuppress/' . $nid : 'manage/sales/suppress/' . $nid, array('query' => drupal_get_destination(), 'attributes' => array('class' => array('btn', 'btn-sm', 'btn-info')))) . ' ' . l(t('Change to "@nodetype"', array('@nodetype' => node_type_get_name($nodetypename))), 'manage/sales/changeprofiletype/' . $nid . '/' . $nodetypename, array('query' => drupal_get_destination(), 'attributes' => array('class' => array('btn', 'btn-sm', 'btn-default')))),
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

    $concat = $form_state['values']['suggestions'];
    foreach($concat as $index => $bool) {
        if(!$bool) unset($concat[$index]);
    }
    drupal_goto('manage/conflate/' . implode(',', $concat));
	
    /*
	rm_user_activity(
		t('Conflated @count suggestions to one: <a href="@url">@profile</a>', array('@count' => count($form_state['values']['suggestions']), '@url' => url('node/' . $mainprofileobject->nid), '@profile' => $mainprofileobject->title)),
		$mainprofileobject->nid,
		'node'
		//$uid
	); */
}

function rm_sales_conflate_profiles_form($form, &$form_state, $default) {
    $nids = explode(',', $default);
    
    $nodeobjects = array();
    foreach($nids as $nid) {
        $nodeobjects[] = node_load($nid);
    }
    
    $form = array();
    
    $form['suggest']['#prefix'] = '<div class="alert alert-danger" role="alert"><h3>Die sind im Begriff die folgenden Profilvorschläge zu <strong>einem einzelnen</strong> zusammenzuführen:</h3><ul>';
    
    foreach($nodeobjects as $nodeobject) {
        $form['suggest']['#prefix'] .= '<li>' . $nodeobject->title . ' (' . $nodeobject->field_address[LANGUAGE_NONE][0]['locality'] . ')</li>';
    }
    
    $form['suggest']['#prefix'] .= '</ul><h3><strong>Bitte beachten Sie, dass in diesem Prozess ein neuer Datensatz mit den Informationen erstellt wird, die Sie unten angeben. Die hier aufgelisteten Datensätze werden danach unwiderruflich gelöscht.</strong></h3></div>';
    
    $form['suggest']['necessary'] = array(
        '#type' => 'fieldset',
        '#title' => t('These infos we need...'),
    );
	$form['suggest']['necessary']['type'] = array(
		'#type' => 'radios',
		'#title' => t('Vendor or gastronomy'),
		'#options' => array(
			'seller_profile' => t('Suggest vendor'),
			'customer_profile' => t('Suggest gastronomy'),
		),
        '#default_value' => $nodeobjects[0]->type,
		'#required' => TRUE,
	);
	
	$form['suggest']['necessary']['title'] = array(
		'#type' => 'textfield',
		'#title' => t('Name of the vendor or gastronomy'),
		'#required' => TRUE,
	);
    
    $first = TRUE;
    
    foreach($nodeobjects as $nodeobject) {
        if(!empty($nodeobject->title)) {
            if($first) {
                $form['suggest']['necessary']['title']['#default_value'] = $nodeobject->title;
                $first = FALSE;
            }
            else {
                $form['suggest']['necessary']['title']['#default_value'] .= ', ' . $nodeobject->title;
            }
        }
    }

	$form['suggest']['necessary']['locality'] = array(
		'#type' => 'textfield',
		'#title' => t('City'),
		'#required' => TRUE,
	);
    
    $first = TRUE;
    foreach($nodeobjects as $nodeobject) {
        if(!empty($nodeobject->field_address[LANGUAGE_NONE][0]['locality'])) {
            if($first) {
                $form['suggest']['necessary']['locality']['#default_value'] = $nodeobject->field_address[LANGUAGE_NONE][0]['locality'];
                $first = FALSE;
            }
            else {
                $form['suggest']['necessary']['locality']['#default_value'] .= ', ' . $nodeobject->field_address[LANGUAGE_NONE][0]['locality'];
            }
        }
    }

    $form['suggest']['nicetohave'] = array(
        '#type' => 'fieldset',
        '#title' => t('... and these infos are helpful'),
    );
    
    $form['suggest']['nicetohave']['thoroughfare'] = array(
		'#type' => 'textfield',
		'#title' => t('Street'),
		'#required' => FALSE,
	);
    
    $first = TRUE;
    foreach($nodeobjects as $nodeobject) {
        if(!empty($nodeobject->field_address[LANGUAGE_NONE][0]['thoroughfare'])) {
            if($first) {
                $form['suggest']['nicetohave']['thoroughfare']['#default_value'] = $nodeobject->field_address[LANGUAGE_NONE][0]['thoroughfare'];
                $first = FALSE;
            }
            else {
                $form['suggest']['nicetohave']['thoroughfare']['#default_value'] .= ', ' . $nodeobject->field_address[LANGUAGE_NONE][0]['thoroughfare'];
            }
        }
    }

    $form['suggest']['nicetohave']['postal_code'] = array(
        '#type' => 'textfield',
        '#title' => t('Postal code'),
        '#required' => FALSE,
    );
    
    $first = TRUE;
    foreach($nodeobjects as $nodeobject) {
        if(!empty($nodeobject->field_address[LANGUAGE_NONE][0]['postal_code'])) {
            if($first) {
                $form['suggest']['nicetohave']['postal_code']['#default_value'] = $nodeobject->field_address[LANGUAGE_NONE][0]['postal_code'];
                $first = FALSE;
            }
            else {
                $form['suggest']['nicetohave']['postal_code']['#default_value'] .= ', ' . $nodeobject->field_address[LANGUAGE_NONE][0]['postal_code'];
            }
        }
    }

	$form['suggest']['nicetohave']['phone'] = array(
		'#type' => 'textfield',
		'#title' => t('Phone'),
		'#required' => FALSE,
	);
    
    $first = TRUE;
    foreach($nodeobjects as $nodeobject) {
        if(!empty($nodeobject->field_address[LANGUAGE_NONE][0]['phone'])) {
            if($first) {
                $form['suggest']['nicetohave']['phone']['#default_value'] = $nodeobject->field_address[LANGUAGE_NONE][0]['phone'];
                $first = FALSE;
            }
            else {
                $form['suggest']['nicetohave']['phone']['#default_value'] .= ', ' . $nodeobject->field_address[LANGUAGE_NONE][0]['phone'];
            }
        }
    }
    
    $counter = 0;
    foreach($nodeobjects as $nodeobject) {
        if(!empty($nodeobject->field_suggester[LANGUAGE_NONE][0]['value'])) {
            foreach($nodeobject->field_suggester[LANGUAGE_NONE] as $sugkey => $sugval) {
                $form['suggest']['nicetohave']['suggester' . $counter] = array(
                    '#type' => 'textfield',
                    '#value' => $sugval['value'],
                );
                $counter++;
            }
        }
    }
    
    $form['suggest']['nicetohave']['tobeconflated'] = array(
        '#type' => 'hidden',
        '#value' => $default,
    );

    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Conflate'),
    );
	
	return $form;
}

function rm_sales_conflate_profiles_form_validate($form, &$form_state) {
	if(!empty($form_state['values']['postal_code'])) {
		if(!is_numeric($form_state['values']['postal_code'])) form_set_error('postal_code', t('Please enter a numeric value as postal code'));
	}
	if(!empty($form_state['values']['phone'])) {
		if(!is_numeric($form_state['values']['phone'])) form_set_error('phone', t('Please enter a numeric value as phone number'));
	}
}

function rm_sales_conflate_profiles_form_submit($form, &$form_state) {
    $nids = explode(',', $form_state['values']['tobeconflated']);
    
    $nodeobjects = array();
    $first = TRUE;
    foreach($nids as $nid) {
        if($first) {
            $mainprofileobject = node_load($nid);
            $first = FALSE;
        }
        else {
            $nodeobjects[] = node_load($nid);
        }
    }
    
    unset($mainprofileobject->field_suggester[LANGUAGE_NONE]);
    foreach($form_state['values'] as $formvalues => $data) {
        $tmp = explode('suggester', $formvalues);
        if(isset($tmp[1])) {
            $mainprofileobject->field_suggester[LANGUAGE_NONE][] = array('value' => $data);
        }
    }
    $mainprofileobject->title = $form_state['values']['title'];
    $mainprofileobject->type = $form_state['values']['type'];
    if(!empty($form_state['values']['locality'])) $mainprofileobject->field_address[LANGUAGE_NONE][0]['locality'] = $form_state['values']['locality'];
    if(!empty($form_state['values']['thoroughfare'])) $mainprofileobject->field_address[LANGUAGE_NONE][0]['thoroughfare'] = $form_state['values']['thoroughfare'];
    if(!empty($form_state['values']['postal_code'])) $mainprofileobject->field_address[LANGUAGE_NONE][0]['postal_code'] = $form_state['values']['postal_code'];
    if(!empty($form_state['values']['phone'])) {
		$mainprofileobject->field_publicphone[LANGUAGE_NONE][0]['number'] = $form_state['values']['phone'];
		$mainprofileobject->field_publicphone[LANGUAGE_NONE][0]['country_codes'] = 'de';
	}
    node_save($mainprofileobject);
    foreach($nodeobjects as $nodeobject) {
        node_delete($nodeobject->nid);
    }
    drupal_set_message(t('Profiles have been conflated'));
    drupal_goto();
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
	if(!rm_api_is_ajax()) drupal_goto();
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
	if(!rm_api_is_ajax()) drupal_goto();
}

function rm_sales_updateofferinfourl($nid) {
    $text = $_POST['text'];
    $profileobject = node_load($nid);
	$profileobject->field_offerinfo_url[LANGUAGE_NONE][0]['value'] = $text;
	node_save($profileobject);
	global $user;
	$userobject = user_load($uid);
	rm_user_activity(
		t('Updated comment on <a href="@url">@profile</a>', array('@url' => url('node/' . $nid), '@profile' => $profileobject->title)),
		$nid,
		'node',
		$user->uid
	);
	if(!rm_api_is_ajax()) drupal_goto();
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
	if(!rm_api_is_ajax()) drupal_goto();
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

function rm_sales_editprofile($form, &$form_state, $nid) {

    global $user;
	$profileobject = node_load($nid);
    
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