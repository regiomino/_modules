/**
 * Created by Martin on 04.08.14.
 */
jQuery(document).ready(function ($) {    
    
    $('.priorities select').change(function() {
        var nidstring = $(this).attr('id');
        var res = nidstring.split("_");
        data = new Object;
        data['nid'] = res[1];
        data['prio'] = $(this).val();

        callback_url = Drupal.settings.basePath + 'manage/sales/changepriority/' + data['nid'] + '/' + data['prio'];
            
        $.ajax({
            url: callback_url,
            type: 'POST',
            data: data
        });
    });
    
    $('.comments textarea').on('blur', function() {
        var nidstring = $(this).attr('id');
        var res = nidstring.split("_");
        data = new Object;
        data['nid'] = res[1];
        data['text'] = $(this).val();

        callback_url = Drupal.settings.basePath + 'manage/sales/updatecomment/' + data['nid'];
            
        $.ajax({
            url: callback_url,
            type: 'POST',
            data: data
        });
    });
    
    $('.nextactions input').on('blur', function() {
        var nidstring = $(this).attr('id');
        var res = nidstring.split("_");
        data = new Object;
        data['nid'] = res[1];
        data['text'] = $(this).val();
        callback_url = Drupal.settings.basePath + 'manage/sales/updatenextaction/' + data['nid'];
        $.ajax({
            url: callback_url,
            type: 'POST',
            data: data
        });
    });
    
    $('.salesguys select').change(function() {
        var nidstring = $(this).attr('id');
        var res = nidstring.split("_");
        data = new Object;
		data['nid'] = res[1];
		data['uid'] = $(this).val();
		callback_url = Drupal.settings.basePath + 'manage/sales/assignprofile/' + data['nid'] + '/' + data['uid'];
            
		$.ajax({
			url: callback_url,
			type: 'POST',
			data: data,
		});
    });

});