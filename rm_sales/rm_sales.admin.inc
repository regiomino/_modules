<?php

function rm_sales_admin_offer_import() {
	//Cart preservation. Maximum number of days, inactive cart items should be preserved
	$form['offer_import'] = array(
		'#type' => 'submit',
		'#value' => t('Import from importfile_offers.xls'),
		'#description' => t('Place importfile_offers.xls in private file directory and make sure it is correctly formatted. Click import only once!'),
	);
    
    return $form;
}

function rm_sales_admin_offer_import_submit() {
	$dir = file_stream_wrapper_get_instance_by_uri('private://')->realpath();
    $filename = 'importfile_offers.xls';
    $path = "$dir/$filename";
    $result = rm_api_excel_import($path);
    global $user;
    $offerarray = array();
    foreach($result as $sheet) {
        foreach($sheet as $line) {            
            if($user->uid == 1) {
                
                $offerarray[$line['Verkäufer UID']]['offer_description'][md5($line['Produkt Titel'])]['Produkt Titel'] = $line['Produkt Titel'];
                $offerarray[$line['Verkäufer UID']]['offer_description'][md5($line['Produkt Titel'])]['Produkt Herkunft Firma'] = $line['Produkt Herkunft Firma'];
                $offerarray[$line['Verkäufer UID']]['offer_description'][md5($line['Produkt Titel'])]['Produkt Herkunft PLZ'] = $line['Produkt Herkunft PLZ'];
                $offerarray[$line['Verkäufer UID']]['offer_description'][md5($line['Produkt Titel'])]['Produkt Herkunft Ort'] = $line['Produkt Herkunft Ort'];
                $offerarray[$line['Verkäufer UID']]['offer_description'][md5($line['Produkt Titel'])]['Produkt Herkunft Land'] = $line['Produkt Herkunft Land'];
                
                $offerarray[$line['Verkäufer UID']]['offer_description'][md5($line['Produkt Titel'])]['offer_variation'][md5($line['Variante Titel'])]['Variante Titel'] = $line['Variante Titel'];
                $offerarray[$line['Verkäufer UID']]['offer_description'][md5($line['Produkt Titel'])]['offer_variation'][md5($line['Variante Titel'])]['Variante Beschreibung'] = $line['Variante Beschreibung'];
                $offerarray[$line['Verkäufer UID']]['offer_description'][md5($line['Produkt Titel'])]['offer_variation'][md5($line['Variante Titel'])]['Variante Angebotsnummer'] = $line['Variante Angebotsnummer'];
                $offerarray[$line['Verkäufer UID']]['offer_description'][md5($line['Produkt Titel'])]['offer_variation'][md5($line['Variante Titel'])]['Variante Produkteinheit Anzahl'] = $line['Variante Produkteinheit Anzahl'];
                $offerarray[$line['Verkäufer UID']]['offer_description'][md5($line['Produkt Titel'])]['offer_variation'][md5($line['Variante Titel'])]['Variante Produkteinheit Einheit'] = $line['Variante Produkteinheit Einheit'];
                $offerarray[$line['Verkäufer UID']]['offer_description'][md5($line['Produkt Titel'])]['offer_variation'][md5($line['Variante Titel'])]['Variante EAN'] = $line['Variante EAN'];
                $offerarray[$line['Verkäufer UID']]['offer_description'][md5($line['Produkt Titel'])]['offer_variation'][md5($line['Variante Titel'])]['Variante Bestand'] = $line['Variante Bestand'];
                $offerarray[$line['Verkäufer UID']]['offer_description'][md5($line['Produkt Titel'])]['offer_variation'][md5($line['Variante Titel'])]['Variante Haltbarkeit'] = $line['Variante Haltbarkeit'];
                $offerarray[$line['Verkäufer UID']]['offer_description'][md5($line['Produkt Titel'])]['offer_variation'][md5($line['Variante Titel'])]['Variante MHD'] = $line['Variante MHD'];
                
                $offerarray[$line['Verkäufer UID']]['offer_description'][md5($line['Produkt Titel'])]['offer_variation'][md5($line['Variante Titel'])]['trading_unit'][md5($line['Gebinde Anzahl'])]['Gebinde Anzahl'] = $line['Gebinde Anzahl'];
                $offerarray[$line['Verkäufer UID']]['offer_description'][md5($line['Produkt Titel'])]['offer_variation'][md5($line['Variante Titel'])]['trading_unit'][md5($line['Gebinde Anzahl'])]['Gebinde Art'] = $line['Gebinde Art'];
                $offerarray[$line['Verkäufer UID']]['offer_description'][md5($line['Produkt Titel'])]['offer_variation'][md5($line['Variante Titel'])]['trading_unit'][md5($line['Gebinde Anzahl'])]['Gebinde Preis'] = $line['Gebinde Preis'];
                $offerarray[$line['Verkäufer UID']]['offer_description'][md5($line['Produkt Titel'])]['offer_variation'][md5($line['Variante Titel'])]['trading_unit'][md5($line['Gebinde Anzahl'])]['Gebinde MwSt'] = $line['Gebinde MwSt'];
                $offerarray[$line['Verkäufer UID']]['offer_description'][md5($line['Produkt Titel'])]['offer_variation'][md5($line['Variante Titel'])]['trading_unit'][md5($line['Gebinde Anzahl'])]['Gebinde Pfand'] = $line['Gebinde Pfand'];
                
            }
        }
    }
    rm_sales_create_offers_from_import($offerarray);
}

function rm_sales_create_offers_from_import($oa) {

    foreach($oa as $suid => $odesc) {
        foreach($odesc['offer_description'] as $od) {
            //Create new offer description
            $offer_description = rm_api_create_new_node('offer_description', node_type_get_name('offer_description'), 'de', $suid, 0, 0, 0, 0);
            
            //Add fields to offer decsription
            $offer_description->title = $od['Produkt Titel'];
            if(!empty($od['Produkt Herkunft Firma'])) $offer_description->field_origin[LANGUAGE_NONE][0]['organisation_name'] = $od['Produkt Herkunft Firma'];
            if(!empty($od['Produkt Herkunft PLZ'])) $offer_description->field_origin[LANGUAGE_NONE][0]['postal_code'] = $od['Produkt Herkunft PLZ'];
            if(!empty($od['Produkt Herkunft Ort'])) $offer_description->field_origin[LANGUAGE_NONE][0]['locality'] = $od['Produkt Herkunft Ort'];
            if(!empty($od['Produkt Herkunft Land'])) $offer_description->field_origin[LANGUAGE_NONE][0]['country'] = $od['Produkt Herkunft Land'];
            
            foreach($od['offer_variation'] as $ov) {
                //Create new offer variation and link it to offer description
                $offer_variation = rm_api_create_new_node('offer_variation', node_type_get_name('offer_variation'), 'de', $suid, 0, 0, 0, 0);
                $offer_description->field_offer_variation_reference[LANGUAGE_NONE][]['target_id'] = $offer_variation->nid;
                
                $offer_variation->title = $ov['Variante Titel'];
                $offer_variation->body[LANGUAGE_NONE][0]['value'] = $ov['Variante Beschreibung'];
                $filepath = drupal_realpath(drupal_get_path('module', 'rm_sales') . '/no-image.png');
                $file = (object) array(
                    'uid' => 1,
                    'uri' => $filepath,
                    'filemime' => file_get_mimetype($filepath),
                    'status' => 1,
                );
                $file = file_copy($file, 'public://');
                $offer_variation->field_image[LANGUAGE_NONE][0] = (array)$file;
                $offer_variation->field_sku[LANGUAGE_NONE][0]['value'] = $ov['Variante Angebotsnummer'];
                $offer_variation->field_productunit[LANGUAGE_NONE][0]['first'] = $ov['Variante Produkteinheit Anzahl'];
                $offer_variation->field_productunit[LANGUAGE_NONE][0]['second'] = $ov['Variante Produkteinheit Einheit'];
                if(!empty($ov['Variante EAN'])) $offer_variation->field_gtin[LANGUAGE_NONE][0]['value'] = $ov['Variante EAN'];
                if(!empty($ov['Variante Vorlaufzeit'])) $offer_variation->field_leadtime[LANGUAGE_NONE][0]['value'] = $ov['Variante Vorlaufzeit'];
                if(!empty($ov['Variante Haltbarkeit'])) $offer_variation->field_use_within[LANGUAGE_NONE][0]['value'] = $ov['Variante Haltbarkeit'];
                if(!empty($ov['Variante MHD'])) $offer_variation->field_expiration_date[LANGUAGE_NONE][0]['value'] = $ov['Variante MHD'];
                
                foreach($ov['trading_unit'] as $tu) {
                    //Create new trading unit and link it to offer variation
                    $trading_unit = rm_api_create_new_node('trading_unit', node_type_get_name('trading_unit'), 'de', $suid, 0, 0, 0, 0);
                    $offer_variation->field_trading_unit_reference[LANGUAGE_NONE][]['target_id'] = $trading_unit->nid;
                    
                    $trading_unit->field_tu_amount[LANGUAGE_NONE][0]['value'] = $tu['Gebinde Anzahl'];
                    $trading_unit->field_tu_price[LANGUAGE_NONE][0]['value'] = $tu['Gebinde Preis'];
                    $trading_unit->field_tu_deposit[LANGUAGE_NONE][0]['value'] = $tu['Gebinde Pfand'];
                    $trading_unit->field_tu_vat[LANGUAGE_NONE][0]['value'] = $tu['Gebinde MwSt'];
                    $trading_unit->field_tu_packaging[LANGUAGE_NONE][0]['value'] = $tu['Gebinde Art'];
                    if(!empty($tu['Gebinde UVP'])) $trading_unit->field_tu_rrp[LANGUAGE_NONE][0]['value'] = $tu['Gebinde UVP'];
                    
                    node_save($trading_unit);
                }
                
                node_save($offer_variation);
            }
            
            node_save($offer_description);
        }
    }
}

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
                $new_node = rm_sales_create_profile_from_import($line, 87);
                rm_sales_publishprofile($new_node->nid, FALSE);
            }
        }
    }
}

function rm_sales_create_profile_from_import($line, $tid) {

    $type = ($line['Status'] == 'Verkäufer') ? 'seller_profile' : 'customer_profile';
    
	$new_node = rm_api_create_new_node(
		$type, //Type
		$line['Profiltitel'], //Title
		'de', //Language
		0, //Uid of author
		0, //Status
		0, //Suppressed
		1, //Sticky
		0, //Promote
        0 //Prospect
	);
    
    if($type == 'seller_profile') {
        $new_node->field_sellercategories[LANGUAGE_NONE][0]['tid'] = $tid;
    }
	
	//Store fields
	$new_node->field_address[LANGUAGE_NONE][0] = array(
		'country' => 'de',
		'administrative_area' => NULL,
		'sub_administrative_area' => NULL,
		'locality' => '',
		'dependent_locality' => '',
		'postal_code' => '',
		'thoroughfare' => '',
		'premise' => '',
		'sub_premise' => NULL,
		'organisation_name' => '',
		'name_line' => NULL,
		'first_name' => NULL,
		'last_name' => NULL,
		'data' => NULL,
	);
	if(!empty($line['Ort']) && $line['Ort'] != 'x') $new_node->field_address[LANGUAGE_NONE][0]['locality'] = $line['Ort'];
	if(!empty($line['Strasse Hausnummer']) && $line['Strasse Hausnummer'] != 'x') $new_node->field_address[LANGUAGE_NONE][0]['thoroughfare'] = trim($line['Strasse Hausnummer']);
	if(!empty($line['PLZ']) && $line['PLZ'] != 'x') $new_node->field_address[LANGUAGE_NONE][0]['postal_code'] = $line['PLZ'];
	if(!empty($line['Telefon-Nummer']) && $line['Telefon-Nummer'] != 'x') {
		$new_node->field_publicphone[LANGUAGE_NONE][0]['number'] = preg_replace("/[^0-9]/","",$line['Telefon-Nummer']);
		$new_node->field_publicphone[LANGUAGE_NONE][0]['country_codes'] = 'de';
	}
    if(!empty($line['Fax']) && $line['Fax'] != 'x') {
		$new_node->field_publicfax[LANGUAGE_NONE][0]['number'] = preg_replace("/[^0-9]/","",$line['Fax']);
		$new_node->field_publicfax[LANGUAGE_NONE][0]['country_codes'] = 'de';
	}
    if(!empty($line['Exakte Firmenbezeichnung lt. Impressum']) && $line['Exakte Firmenbezeichnung lt. Impressum'] != 'x') {
		$new_node->field_company_name[LANGUAGE_NONE][0]['value'] = trim($line['Exakte Firmenbezeichnung lt. Impressum']);
	}
    else {
        $new_node->field_company_name[LANGUAGE_NONE][0]['value'] = trim($line['Profiltitel']);
    }
    
    if(!empty($line['Quelle']) && $line['Quelle'] != 'x') {
		$new_node->field_source_url[LANGUAGE_NONE][0]['value'] = trim($line['Quelle']);
	}
    $new_node->field_salescomment[LANGUAGE_NONE][0]['value'] = '';

    if(!empty($line['URL']) && $line['URL'] != 'x') {
		$new_node->field_salescomment[LANGUAGE_NONE][0]['value'] .= 'URL: ' . trim($line['URL']) . '<br>';
	}
    if(!empty($line['E-Mail']) && $line['E-Mail'] != 'x') {
		$new_node->field_email[LANGUAGE_NONE][0]['email'] = trim($line['E-Mail']);
	}
    
    
    if(!empty($line['Kurzbeschreibung des Profils']) && $line['Kurzbeschreibung des Profils'] != 'x') {
		$new_node->body[LANGUAGE_NONE][0]['value'] = trim($line['Kurzbeschreibung des Profils']);
	}
    else {
        $new_node->body[LANGUAGE_NONE][0]['value'] = trim($line['Profiltitel']);
    }
    
    
    if(!empty($line['Kontoinhaber']) && $line['Kontoinhaber'] != 'x') {
		$new_node->field_bankaccountholder[LANGUAGE_NONE][0]['value'] = trim($line['Kontoinhaber']);
	}
    else {
        $new_node->field_bankaccountholder[LANGUAGE_NONE][0]['value'] = trim($line['Profiltitel']);
    }
    
    if(!empty($line['IBAN']) && $line['IBAN'] != 'x') {
		$new_node->field_iban[LANGUAGE_NONE][0]['value'] = trim($line['IBAN']);
	}
    else {
		$new_node->field_iban[LANGUAGE_NONE][0]['value'] = 0;
	}
    
    if(!empty($line['BIC']) && $line['BIC'] != 'x') {
		$new_node->field_bic[LANGUAGE_NONE][0]['value'] = trim($line['BIC']);
	}
    else {
		$new_node->field_bic[LANGUAGE_NONE][0]['value'] = 0;
	}
    
    
    $filepath = drupal_realpath(drupal_get_path('module', 'rm_sales') . '/no-image.png');
    $file = (object) array(
        'uid' => 1,
        'uri' => $filepath,
        'filemime' => file_get_mimetype($filepath),
        'status' => 1,
    );
    $file = file_copy($file, 'public://');
    $new_node->field_image[LANGUAGE_NONE][0] = (array)$file;
    
    $new_node->field_next_action[LANGUAGE_NONE][0]['value'] = 1924988399;
    
    if(!empty($line['UStID / StNr']) && $line['UStID / StNr'] != 'x') {
		$new_node->field_taxnumber[LANGUAGE_NONE][0]['value'] = substr(preg_replace('/[^A-Za-z0-9-.\/]/', '', $line['UStID / StNr']), 0, 14);
	}
    if(!empty($line['URL zu Angebotsdaten']) && $line['URL zu Angebotsdaten'] != 'x') {
		$new_node->field_salescomment[LANGUAGE_NONE][0]['value'] .= 'Angebote: ' . $line['URL zu Angebotsdaten'];
	}
    
    if(!empty($line['Rechnungsadresse = Lieferanschrift']) && $line['Rechnungsadresse = Lieferanschrift'] == 'x') {
        $new_node->field_billingaddress[LANGUAGE_NONE][0] = array(
            'country' => 'de',
            'administrative_area' => NULL,
            'sub_administrative_area' => NULL,
            'locality' => '',
            'dependent_locality' => '',
            'postal_code' => '',
            'thoroughfare' => '',
            'premise' => '',
            'sub_premise' => NULL,
            'organisation_name' => '',
            'name_line' => NULL,
            'first_name' => NULL,
            'last_name' => NULL,
            'data' => NULL,
        );
        if(!empty($line['Ort']) && $line['Ort'] != 'x') $new_node->field_billingaddress[LANGUAGE_NONE][0]['locality'] = trim($line['Ort']);
        if(!empty($line['Strasse Hausnummer']) && $line['Strasse Hausnummer'] != 'x') $new_node->field_billingaddress[LANGUAGE_NONE][0]['thoroughfare'] = trim($line['Strasse Hausnummer']);
        if(!empty($line['PLZ']) && $line['PLZ'] != 'x') $new_node->field_billingaddress[LANGUAGE_NONE][0]['postal_code'] = trim($line['PLZ']);
	}
    
    
    
	
	if(!empty($line['Empfehler 1']) && $line['Empfehler 1'] != 'x') {
        $new_node->field_suggester[LANGUAGE_NONE][] = array(
            'value' => trim($line['Empfehler 1'])
        );
    }
    
    if(!empty($line['Empfehler 2']) && $line['Empfehler 2'] != 'x') {
        $new_node->field_suggester[LANGUAGE_NONE][] = array(
            'value' => trim($line['Empfehler 2'])
        );
    }
    
    if(!empty($line['Empfehler 3']) && $line['Empfehler 3'] != 'x') {
        $new_node->field_suggester[LANGUAGE_NONE][] = array(
            'value' => trim($line['Empfehler 3'])
        );
    }
    
    if(!empty($line['Empfehler 4']) && $line['Empfehler 4'] != 'x') {
        $new_node->field_suggester[LANGUAGE_NONE][] = array(
            'value' => trim($line['Empfehler 4'])
        );
    }
    
    if(!empty($line['Empfehler 5']) && $line['Empfehler 5'] != 'x') {
        $new_node->field_suggester[LANGUAGE_NONE][] = array(
            'value' => trim($line['Empfehler 5'])
        );
    }
    
    if(!empty($line['Empfehler 6']) && $line['Empfehler 6'] != 'x') {
        $new_node->field_suggester[LANGUAGE_NONE][] = array(
            'value' => trim($line['Empfehler 6'])
        );
    }
    
    if(!empty($line['Empfehler 7']) && $line['Empfehler 7'] != 'x') {
        $new_node->field_suggester[LANGUAGE_NONE][] = array(
            'value' => trim($line['Empfehler 7'])
        );
    }
    
    if(!empty($line['Empfehler 8']) && $line['Empfehler 8'] != 'x') {
        $new_node->field_suggester[LANGUAGE_NONE][] = array(
            'value' => trim($line['Empfehler 8'])
        );
    }
	
    $new_node->field_priority[LANGUAGE_NONE][0]['value'] = 9;
    $new_node->field_next_action[LANGUAGE_NONE][0]['value'] = 1924988399;
    $new_node->field_salesguy[LANGUAGE_NONE][0]['target_id'] = 1;
	node_save($new_node);
	return $new_node;
	
}