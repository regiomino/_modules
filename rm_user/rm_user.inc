<?php

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

function rm_user_editaccount($form, &$form_state) {
    $form = array();
    
    return $form;
}