/**
 * Created by Martin on 04.08.14.
 */
jQuery(document).ready(function ($) {  

    $.fn.addThrobber = function(element) {
        element.append('<div class="ajax-progress ajax-progress-throbber"><div class="throbber">&nbsp;</div></div>');
    }
    
    $.fn.removeThrobber = function(element) {
        element.find('.ajax-progress-throbber').remove();
    }
    
    $('.priorities select').change(function() {
        var throbberElement = $(this).parent();
        $.fn.addThrobber(throbberElement);
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
        }).done(function() {
            $.fn.removeThrobber(throbberElement);
        });
    });
    
    $('.comments textarea').on('input', function() {
        var throbberElement = $(this).parent();
        $.fn.addThrobber(throbberElement);
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
        }).done(function() {
            $.fn.removeThrobber(throbberElement);
        });
    });
    
    $('.nextactions input').on('input', function() {
        var throbberElement = $(this).parent();
        $.fn.addThrobber(throbberElement);
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
        }).done(function() {
            $.fn.removeThrobber(throbberElement);
        });
    });
    
    $('.salesguys select').change(function() {
        var throbberElement = $(this).parent();
        $.fn.addThrobber(throbberElement);
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
		}).done(function() {
            $.fn.removeThrobber(throbberElement);
        });
    });
    
    $('#filterSuggestions').keyup(function () {
        var rex = new RegExp($(this).val(), 'i');
        $('form table tbody tr').hide();
        $('form table tbody tr').filter(function () {
            return rex.test($(this).text());
        }).show();
    });

});