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
    
    $('.product-cart-variations').click(function(e) { e.preventDefault(); }).popover({
      trigger: 'click',
      html: true
    })
    
    $('.dropdown-variation li').click(function(e) {
        e.preventDefault();
        var selected = $(this).text();
        var variationnid = $(this).data('variation-nid');
        $(this).parent().parent().find('.dropdown-toggle').html('<strong>' + selected + '</strong> <span class="caret"></span>');
        
        //Richtigen Produktbody und Warenkorb Link einblenden und andere ausblenden
        $(this).parent().parent().parent().parent().parent().parent().find('.product-body').addClass('hidden');
        $(this).parent().parent().parent().parent().parent().parent().find('.product-cart').addClass('hidden');
        $(this).parent().parent().parent().parent().parent().parent().find('.product-body-' + variationnid).removeClass('hidden');
        $(this).parent().parent().parent().parent().parent().parent().find('.product-cart-' + variationnid).removeClass('hidden');
    });
    
    $('.quantity-select').on('change', function() {
        var option = $(this).val();
        var a_href = $(this).parent().find('.product-cart').attr('href');
        var elements = a_href.split('/');
        var subelements = elements[5].split('?');
        subelements[0] = option;
        elements[5] = subelements.join('?');
        $(this).parent().find('.product-cart').attr('href', elements.join('/'));
    });

});