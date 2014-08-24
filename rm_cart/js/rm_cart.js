jQuery(document).ready(function ($) {
    
    var rmCart = function() {
        this.$cartContainer = $('.grid-l');
        this.$cart = $('#cart');
        this.$add2CartButtons = $('.add2Cart');
        this.timeout;
        this.CART_ADD_STEP = 1;
        this.DELETE_ITEM_CLASS_NAME = '.delete-item';
        this.STEPPER_CONTROLS_CLASS_NAME = '.stepper-control';
        this.STEPPER_DEBOUNCE_VALUE = 270;
        this.STEPPER_CONTROLS_CLASS_NAME = '.stepper-control';
        this.STEPPER_QTY_INPUT_CLASS_NAME ='.stepper-qty';
    };
    
    rmCart.prototype = {
        
        init : function() {
            var _self = this;
            _self.setEventHandlers();
        },
        
        setEventHandlers : function() {
            var _self = this;
          
            // Add2Cart
            _self.$add2CartButtons.on('click.add2Cart', function(e) {
                e.preventDefault();
                var $el = $(this),
                    data = _self.getItemData($el);
                    
                //_self.handleButtonDisplay(),
                _self.addToCart(data);
            });
            
            //Remove
            _self.$cartContainer.on('click.removeFromCart', '' + _self.DELETE_ITEM_CLASS_NAME + '', function(e){
                e.preventDefault();
                var $el = $(this),
                    pid = $el.attr('href');
               
                    _self.removeItemFromCart(pid);
            });
            
            // Stepper + und -
            _self.$cartContainer.on('click.stepperChange', '' + _self.STEPPER_CONTROLS_CLASS_NAME + '', function(e){
                e.preventDefault();
                var $el = $(this),
                    keepOn = true,
                    $cont  =  $el.parents('.cart-item'),
                    $qty = $el.closest('.stepper').find('input.stepper-qty');
                    
                $qty.val( function(i, value) {
                    
                    if ($el.data('operation') == -1 && value == 1 ) {
                        $el.attr('disabled',true);
                        keepOn = false;
                        _self.removeItemFromCart($cont.data('offerid'));
                        clearTimeout(_self.timeout);
                        return +value + (1 * +$el.data('operation'));
                        
                    } else { 
                        return +value + (1 * +$el.data('operation'));
                    }
                });
                if(keepOn) {
                   
                    var amount = $qty.val();
                    
                    // debounce things
                    clearTimeout(_self.timeout);
                                            
                   _self.timeout = setTimeout(function(){
                        _self.timeout = null;
                        
                        var data = {
                            offerid :  parseInt($cont.data('offerid'),10),
                            variation : parseInt($cont.data('variation'),10),
                            tradingunit : parseInt($cont.data('tradingunit'),10),
                            amount : parseInt($qty.val(),10),
                            add : 0
                        }
                          
                        _self.addToCart(data);
                    },_self.STEPPER_DEBOUNCE_VALUE);
                }
            });

             // Stepper Input Focus
            _self.$cartContainer.on('focus.inputqty', '' + _self.STEPPER_QTY_INPUT_CLASS_NAME + '', function(e){
                var $el = $(this);
                $el.mouseup(function(e) { return false; });
                $el.select();
                
                inputValCache = $el.val();
            });
            
            // Stepper Input Change
            _self.$cartContainer.on('change.inputqty', '' + _self.STEPPER_QTY_INPUT_CLASS_NAME + '', function(e){
                var $el = $(this),
                    inputVal = $el.val(),
                    numeric = /^[0-9]+$/.test(inputVal);
                  
                if (numeric) {
                   var $cont  =  $el.parents('.cart-item'),
                        offerid =$cont.data('offerid');
                    if(inputVal == 0) {
                        _self.removeItemFromCart(offerid);
                        return false;
                    }
                    var data = {
                            offerid :  parseInt($cont.data('offerid'),10),
                            variation : parseInt($cont.data('variation'),10),
                            tradingunit : parseInt($cont.data('tradingunit'),10),
                            amount : inputVal,
                            add : 0
                        }
                    
                    _self.addToCart(data);
                    
                } else {
                    $el.val(inputValCache) ;
                }
            });
        },
        
        getItemData : function($el){
            var _self = this;
                tu = $el.parent().siblings('.product-data').find('input[checked]').data('tradingunit'),
                item_data = {
                    offerid : parseInt($el.data('offerid'),10),
                    variation : parseInt($el.data('variation'),10),
                    tradingunit : parseInt(tu,10),
                    amount : _self.CART_ADD_STEP,
                    add : 1
                };
                
            return item_data;
        },
        
        addToCart : function(item_data, uid) {
            var _self = this,
                callback_url = Drupal.settings.basePath + 'addtocart/' + item_data.offerid + '/' + item_data.variation + '/' + item_data.tradingunit + '/' + item_data.amount + '/' + item_data.add;
            data = {};
            
            if(typeof uid !== 'undefined') {
                data['uid'] = uid;
            }
            
            $.ajax({
                url: callback_url,
                type: 'POST',
                data: data
            }).done(function() {
                _self.updateCart();
            }); 
        },
        // aktualisierte Darstellung
        updateCart : function(){
            var _self = this,
                data = {};
            data['module'] = 'rm_cart';
            data['block'] = 'rm_cart_block';
            var callback_url = Drupal.settings.basePath + 'invokeblock';
            
            $.ajax({
                url: callback_url,
                type: 'POST',
                data: data,
                success: function(data) {
                    _self.$cart.html(data);
                }
            });
        },
    
        removeItemFromCart : function(pid,uid) {
            var _self = this,
                callback_url = Drupal.settings.basePath + 'removefromcart/' + pid,
                data = {};
                
            if(typeof uid !== 'undefined') {
                data['uid'] = uid;
            }
                 
            $.ajax({
                url: callback_url,
                type: 'POST',
                data: data
                
            }).done(function() {
             
                _self.updateCart();
            }); 
        }
    }
    
        var cart = new rmCart();
            cart.init();
 
});




/**
 * Created by Martin on 04.08.14.
 



jQuery(document).ready(function ($) {
    
    $.fn.addThrobber = function(element) {
        element.append('<div class="ajax-progress ajax-progress-throbber"><div class="throbber">&nbsp;</div></div>');
    }
    
    $.fn.removeThrobber = function(element) {
        element.find('.ajax-progress-throbber').remove();
    }
    
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
            //refresh cart display
            $.fn.refreshCart();
        });
	}
    
    $.fn.rmRemoveFromCart = function(cart_id, uid) {
        //callback_url recognizes cart_id parameter in path.
        callback_url = Drupal.settings.basePath + 'removefromcart/' + cart_id;
        data = new Object;
        //asynchronous removing of new cart items
        $.ajax({
            url: callback_url,
            type: 'POST',
            data: data
        }).done(function() {
            //refresh cart display
            $.fn.refreshCart();
        });
	}
    
    $.fn.refreshCart = function() {
        data = new Object;
        data['module'] = 'rm_cart';
        data['block'] = 'rm_cart_block';
        var callback_url = Drupal.settings.basePath + 'invokeblock';
        $.ajax({
            url: callback_url,
            type: 'POST',
            data: data,
            success: function(data) {
                $('.cart-display').html(data);
            }
        });
    }
   
    $(document.body).on('click', '.cart-remove-link', function(e) {
        //usually the cart item is added through a page reload. this must be prevented
        e.preventDefault();
        $.fn.addThrobber($(this));
        //collect necessary parameters through href-attribute of the clicked link
        var a_href = $(this).attr('href');
        //split href
        var elements = a_href.split('/');
        //call rmAddToCart
        $.fn.rmRemoveFromCart(elements[2]);
    });
    
     $('.product-cart').on('click', function(e) {
        //usually the cart item is added through a page reload. this must be prevented
        e.preventDefault();
        //collect necessary parameters through href-attribute of the clicked link
        var a_href = $(this).attr('href');
        //split href
        var elements = a_href.split('/');
        //call rmAddToCart
        $.fn.rmAddToCart(elements[2], elements[3], elements[4], elements[5]);
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
*/