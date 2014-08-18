/**
 * Created by Martin on 04.08.14.
 */
jQuery(document).ready(function ($) {    
    
    $.fn.rmAddToCart = function(offer_description, offer_variation, trading_unit, quantity, uid) {
        //callback_url recognizes all parameters in path besides uid. uid is always the one of the current user.
        callback_url = Drupal.settings.basePath + 'addtocart/' + offer_description + '/' + offer_variation + '/' + trading_unit + '/' + quantity;
        
        //still a custom uid can be provided to this function. this needs to be passed on through POST.
        //the post is only processed by the php function if the acting user is admin. otherwise the uid
        //of the current user is taken. this makes it possible for admins to add items to a different user.
		data = new Object;
        if(typeof uid !== 'undefined') {
            data['uid'] = uid;
        }
        
        //asynchronous adding of new cart items
        $.ajax({
            url: callback_url,
            type: 'POST',
            data: data
        }).done(function() {
            alert('success');
        });
	}
    
    $('.product-cart').on('click', function(e) {
        //usually the cart item is added through a page reload. this must be prevented
        e.preventDefault();
        //collect necessary parameters through href-attribute of the clicked link
        var a_href = $(this).attr('href');
        //split href
        var elements = a_href.split('/');
        //call rmAddToCart
        $.fn.rmAddToCart(elements[2], elements[3], elements[4], elements[5]);
        
        //irgendeine methode zum refreshen der warenkorb darstellung rechts muss hier gecalled werden
    });

});