jQuery(document).ready(function ($) {

var RC = RC || {};
window.RC = RC;  

RC.init = function () {
    var _self = this;
    _self.sidebar.init();
    _self.products.init();
}

/*
Affix
*/

RC.sidebar =  {};
RC.sidebar.$sidebar = $('#flexfix-sidebar');
RC.sidebar.$cartToggle = $('#cart-toggle');
RC.sidebar.$itemDisplay = $('#item-amount');

RC.sidebar.init = function(){
    var _self = this;
    _self.updateItemAmount();
    _self.addListeners();
    _self.addAffix();
    
};

RC.sidebar.addAffix = function(){
    var _self = this;

    RC.products.$cartContainer.affix({
        offset: {
            top: 49,

            bottom:  function () {
              return (this.bottom = $('.footer').outerHeight(true) +80);
            }
        }
    });
};

RC.sidebar.addListeners = function(){
    var _self = this;

    _self.$cartToggle.on('click.cartToggle', function() {
        $(this).toggleClass('active');
        _self.$sidebar.toggleClass('active');
    });
}

RC.sidebar.updateItemAmount = function(){
    var _self = this;
    var items = RC.products.$cartContainer.find('.cart-item').length;
    if (items > 0) {
        _self.$itemDisplay.text(items);
    } else {
        _self.$itemDisplay.text('0');
    }
}

/*
Products
*/

RC.products = {};
RC.products.$cont = $('#product-grid-container');
RC.products.$cartContainer = $('.cart-container');
RC.products.$items = $('.product-item', RC.products.$cont);
RC.products.offers = {};
RC.products.$addToCart = RC.products.$items .find('button.add2Cart');
RC.products.$plusBtn =  RC.products.$items.find('button.addItem');
RC.products.$minusBtn =  RC.products.$items.find('button.removeItem'); 
RC.products.$dropdownMenuLi = $( '.dropdown-menu li', RC.products.$items );
RC.products.buttonDefaultHTML = '<span class="fa fa-shopping-cart"></span> <span class="fa add2cart-animation fa-check-circle hidden"></span> in den Warenkorb';
RC.products.buttonActiveHTML = ' im <span class="fa fa-shopping-cart"></span>';
RC.products.buttonActive_CLASS_NAME = 'filled';
RC.products.addToCartArea_CLASS = '.add-to-cart-area';
RC.products.CART_ADD_STEP = 1;
RC.products.DELETE_ITEM_CLASS = '.delete-item';
RC.products.STEPPER_CONTROLS_CLASS = '.stepper-control';
RC.products.STEPPER_QTY_INPUT_CLASS ='.stepper-qty';
RC.products.LOADER_CLASS_NAME ='loader';
RC.products.inputValCache = null;




RC.products.buttonDisplay = function(data_attr, prop_name, new_val ){

    $( "[data-" + data_attr +"]" ).each( function() {

        var $el= $( this );
        var $cartArea = $el.find(RC.products.addToCartArea_CLASS);
        var $add2CartButton = $('button.add2Cart',$cartArea);
    
        $el.attr('data-'+prop_name, new_val);

        if (new_val === 0) {
           $cartArea.removeClass(RC.products.buttonActive_CLASS_NAME);
           $add2CartButton.html(RC.products.buttonDefaultHTML);
        }

        else {
            if (!$cartArea.hasClass(RC.products.buttonActive_CLASS_NAME)) {
                $cartArea.addClass(RC.products.buttonActive_CLASS_NAME);
            }

            $add2CartButton.html(''+ new_val + ' '+ RC.products.buttonActiveHTML);
        }
    });
};

RC.products.DataBinder = function( object_id ) {
  var pubSub = $({});
  var data_attr = "bind-" + object_id,
      message = object_id + ":change";
 
  pubSub.on( message, function( evt, prop_name, new_val, ajaxUpdate ) {
    
    if (ajaxUpdate) { 
        RC.ajax.updateCart(object_id, function(){
            RC.products.buttonDisplay(data_attr, prop_name, new_val );

        }  );

    } else {
        RC.products.buttonDisplay(data_attr, prop_name, new_val );
    }
  });

  return pubSub;
};

RC.products.init = function(){
    var _self = this;
    _self.buildProducts();
    _self.addListeners();
};


RC.products.buildProducts = function(){
    var _self = this;

    _self.$items.each(function() {
        var $el  = $(this),
            offerid = $el.data('offerid'),
            variation = $el.data('variation'),
            visibletradingunit = $el.find('.label-area > .price').data('tradingunit'),
            ca = parseInt($el.data('currentamount'),10);

            _self.offers[ variation  ] = new _self.Item(offerid, variation, visibletradingunit);
            _self.offers[ variation  ].set('currentamount', ca, false);
    });
};

RC.products.Item = function( offerid, variation, visibletradingunit ) {
    
    var binder = new RC.products.DataBinder(variation);
    var step = 1;
     
    /*var $item = $('#'+ offerid);
    var $addToCart = $item.find('button.add2Cart');
    var $plusBtn =  $item.find('button.addItem');
    var $minusBtn =  $item.find('button.removeItem');*/

    var item = {
        props : {
            offerid : offerid,
            variation : variation,
            visibletradingunit : visibletradingunit
        },

        set : function(attr_name, val, ajaxUpdate) {
            this.props[ attr_name ] = val;
            if (attr_name === "currentamount") { 
                binder.trigger(variation + ":change", [attr_name, val, ajaxUpdate, this ] ); 
            }
        },

        get : function(attr_name) {
            return this.props[ attr_name ];
        },

        getAll : function() {
            return this.props;
        },

        increase : function(){
            var newVal = this.get('currentamount') + 1;
            this.set('currentamount', newVal, true);
        },

        decrease : function(){
            var newVal = this.get('currentamount') - 1 ;
            this.set('currentamount', newVal, true) ;
        }
    };

    return item;
}

RC.products.addListeners = function() {
    var _self = this;
     
    _self.$addToCart.on('click.add2Cart',{obj: _self}, _self.handleAdd2Cart); 
    _self.$plusBtn.on('click.plus',{obj: _self}, _self.handlePlus); 
    _self.$minusBtn.on('click.minus',{obj: _self}, _self.handleMinus); 
    _self.$dropdownMenuLi.on('click.dropdown',{obj: _self}, _self.handleDropdownSwitch);

    

    //Shopping Cart

    _self.$cartContainer.on('click.stepperChange', '' + _self.STEPPER_CONTROLS_CLASS + '', {obj: _self}, _self.handleStepperClick);
    _self.$cartContainer.on('focus.inputqty', '' + _self.STEPPER_QTY_INPUT_CLASS + '', {obj: _self}, _self.handleinputFocus);  
    _self.$cartContainer.on('change.inputqty', '' + _self.STEPPER_QTY_INPUT_CLASS + '', {obj: _self}, _self.handleinputChange);  
    _self.$cartContainer.on('click.removeFromCart', '' + _self.DELETE_ITEM_CLASS + '',{obj: _self}, _self.handleDeleteClick);
   
};

RC.products.addLoader = function($el){
    var _self = this;
    $el.append('<div class="' + _self.LOADER_CLASS_NAME + '""></div>');
};

RC.products.handleDeleteClick = function(e) {
    e.preventDefault();
    var _self = e.data.obj;
    var $el = $(this);
    var $cont = $el.parents('.cart-item');
    var variation = $cont.data('variation');
    _self.offers[variation].set('currentamount', 0,true);
    _self.addLoader($cont);
}

RC.products.handleinputChange = function(e) {
    var _self = e.data.obj;
    var $el = $(this);
    var inputVal = parseInt($el.val(),10); 
    var numeric = /^[0-9]+$/.test(inputVal);
    var maxItems = parseInt($el.attr('max'),10);
        
    if ( (numeric) && (inputVal <= maxItems)  ) {
       
        var $cont  =  $el.parents('.cart-item');
        var variation = $cont.data('variation');

        _self.offers[variation].set('currentamount', inputVal,true);
        _self.addLoader($cont);
    } else {
        
        $el.val(_self.inputValCache) ;
    }
}

RC.products.handleinputFocus = function(e) {
    var _self = e.data.obj;
    var $el = $(this);
    $el.mouseup(function(e) { return false; });
    $el.select();
    _self.inputValCache= $el.val();
}

RC.products.handleStepperClick = function(e) {
    e.preventDefault();
    var _self = e.data.obj;
    var $el = $(this);
    var $cont  =  $el.parents('.cart-item');
    var variation = parseInt($cont.data('variation'),10);
    var $qty = $el.closest('.stepper').find('input.stepper-qty');
    var qtyVal = parseInt($qty.val(),10);
    var items =  qtyVal + (1 * $el.data('operation'));
    _self.offers[variation].set('currentamount', items,true);
    _self.addLoader($cont);
};

RC.products.handleDropdownSwitch = function(e){
    var _self = e.data.obj;
    var $el = $(this);
    var $anchor = $el.find('a');
    var variation = $(this).closest('.product-item').data('variation');
    var selectedTradingUnit = $anchor.find('.price').data('tradingunit');


     _self.offers[variation].set('visibletradingunit', selectedTradingUnit,false);
     //set amount

    var newContent = $anchor.html();
        
    $el.closest( '.btn-group' )
        .find('.label-area').html(newContent)
        .end()
        .children( '.dropdown-toggle' ).dropdown( 'toggle' );
    $el.siblings().removeClass('hidden');
    $el.addClass('hidden');
    
    return false;
};

RC.products.handleAdd2Cart = function(e){
    var _self = e.data.obj;
    var $el = $(this);
    var variation = $el.parents('.product-item').data('variation');
    _self.offers[variation].set('currentamount', 1,true);
};

RC.products.handlePlus = function(e){
    var _self = e.data.obj;
    var $el = $(this);
    var variation = $el.parents('.product-item').data('variation');
     _self.offers[variation].increase();
};

RC.products.handleMinus = function(e) {
    var _self = e.data.obj;
    var $el = $(this);
    var variation = $el.parents('.product-item').data('variation');
    _self.offers[variation].decrease();
};

RC.ajax = {};

RC.ajax.updateCart = function(offerid, callback, uid) {
    var _self  = this;

    var item_data = {
        offerid : RC.products.offers[offerid].get('offerid'),
        variation : RC.products.offers[offerid].get('variation'),
        tradingunit :RC.products.offers[offerid].get('visibletradingunit'),
        amount : RC.products.offers[offerid].get('currentamount'),
        add : 0
    };

    var callback_url = Drupal.settings.basePath + 'addtocart/' + item_data.offerid + '/' + item_data.variation + '/' + item_data.tradingunit + '/' + item_data.amount + '/' + item_data.add;

    var data = {};
    if(typeof uid !== 'undefined') {
        data['uid'] = uid;
    }

    $.ajax({
        url: callback_url,
        type: 'POST',
        data: data
    }).done(function() {
        _self.injectCartHtml(callback);
        
    });

};

RC.ajax.injectCartHtml = function(cb){
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
            
            RC.products.$cartContainer.html(data);
            RC.sidebar.updateItemAmount();
            cb();
           
        }
    });
};
 
RC.init();

 
 
   


                
});
