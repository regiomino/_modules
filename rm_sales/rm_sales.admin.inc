<?php

/**
 * Form builder.
 * @ingroup forms
 * @see system_settings_form()
 */
function rm_sales_admin_profile_import() {
	//Cart preservation. Maximum number of days, inactive cart items should be preserved
	$form['profile_import'] = array(
		'#type' => 'submit',
		'#value' => t('Import from importfile.xls'),
		'#description' => t('Place importfile.xls in private file directory and make sure it is correctly formatted. Click import only once!'),
	);
    
    return $form;
}

function rm_sales_admin_profile_import_submit() {
	$dir = file_stream_wrapper_get_instance_by_uri('private://')->realpath();
    $filename = 'importfile.xls';
    $path = "$dir/$filename";
    $result = rm_api_excel_import($path);
    global $user;
    foreach($result as $sheet) {
        foreach($sheet as $line) {            
            if($user->uid == 1) {
                $new_node = rm_sales_create_profile_from_import($line, 42);
                rm_sales_publishprofile($new_node->nid, FALSE);
            }
        }
    }
}