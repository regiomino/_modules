jQuery(document).ready(function ($) {
 

var RC = RC || {};
window.RC = RC;  
RC.debounce = function(func, wait, immediate) {
    var timeout;
    return function() {
            var context = this, args = arguments;
            clearTimeout(timeout);
            timeout = setTimeout(function() {
                    timeout = null;
                    if (!immediate) func.apply(context, args);
            }, wait);
            if (immediate && !timeout) func.apply(context, args);
    };
};

RC.getViewport  = function () {
    var e = window, a = 'inner';
    if (!('innerWidth' in window )) {
        a = 'client';
        e = document.documentElement || document.body;
    }
    return { width : e[ a+'Width' ] , height : e[ a+'Height' ] };
};

RC.getViewportName = function () {
    
    var vp = RC.getViewport();
    
    if( vp.width <=991) {
        return "mobile";
    }
    
    else if( vp.width > 991 ) {
        
        return "desktop";
    }
    
}

RC.windowSize = RC.getViewportName();

RC.init = function () {
    var _self = this;
    _self.sidebar.init();
    _self.products.init();
    _self.ta.init();
}

RC.ta = {};
RC.ta.bloodhounds = {};
RC.ta.$clear = $('#clearQuery');
RC.ta.$searchSubmit = $('#searchSubmit');
RC.ta.$ta_input;
RC.ta.$productGrid = $('#product-grid-container').find('.product-grid');

RC.ta.init = function(){
    var _self = this;
    _self.initBloodhounds();
    _self.initTypeahead();
    _self.addListeners();
};

RC.ta.addListeners = function(){
    var _self = this;
    
    
    _self.$ta_input.on('typeahead:selected',function(evt,data){
         _self.filterStuff(data.value);
    });
     
    _self.$ta_input.on('keyup.typeah',{obj: _self},_self.keyUp);
    _self.$clear.on('click.clear',$.proxy(_self.clearInput,_self));
    _self.$searchSubmit.on('click.submit',$.proxy(_self.submit,_self));
};

RC.ta.submit = function(){
    var _self = this;
    var string = _self.$ta_input.val().trim();
    
    if (string.length > 0) {
         _self.filterStuff(string);
    }
    else {return;}
};

RC.ta.keyUp = function(e){
    var _self = e.data.obj;
    var $el = $(this);
    
    var string = $el.val().trim();
    
    if (string.length > 0) {
        _self.$clear.show();
            if(e.which == 13) {
               _self.filterStuff(string);
            }
    } else {
        _self.$clear.hide();
        _self.filterStuff('');
    }
};

RC.ta.clearInput = function(){
    var _self = this;
    _self.$ta_input.val('');
    _self.filterStuff('');
    _self.$clear.hide();
};

RC.ta.filterStuff = function(val) {
    var _self = this;
    var rex = new RegExp(val, 'i');
    _self.$productGrid.find('.grid-item').hide();
    _self.$productGrid.find('.grid-item').filter(function () {
        return rex.test($(this).text());
    }).show();

};

RC.ta.initBloodhounds = function(){
    var _self = this;
    
    _self.bloodhounds.products = new Bloodhound({
        datumTokenizer: Bloodhound.tokenizers.obj.whitespace('value'),
        queryTokenizer: Bloodhound.tokenizers.whitespace,
        local: $.map(Drupal.settings.rm_shop.products, function(v) { return { value: v }; }),
        limit  : 7
    });
    
    _self.bloodhounds.products.initialize();
  
};

RC.ta.initTypeahead = function(){
    var _self = this;
    
    _self.$ta_input = $('#filterProducts').typeahead({
            hint: true,
            highlight: true,
            minLength: 1
        },
        {
            name: 'products',
            displayKey: 'value',
            source: _self.bloodhounds.products.ttAdapter()
        }
    );
};

RC.sidebar =  {};
RC.sidebar.navBarHeight = 65;
RC.sidebar.breadCrumbsHeight = 47;
RC.sidebar.chromeHeight = RC.sidebar.navBarHeight + RC.sidebar.breadCrumbsHeight;
RC.sidebar.$sidebar = $('#flexfix-sidebar');
RC.sidebar.$cartToggle = $('#cart-toggle');
RC.sidebar.$itemDisplay = $('#item-amount');

RC.sidebar.getWindowH = function(){
    var vp = RC.getViewport();
    return vp.height;
};

RC.sidebar.getVisibleSidebarH = function(){
     var _self = this;
     var h = _self.getWindowH() - _self.chromeHeight ;
     return h;
};

RC.sidebar.getCartHeight = function() {
    return RC.products.$cartContainer.height();
};

RC.sidebar.cartTooHigh = function(){
    var _self = this;
    var sidebarArea = _self.getVisibleSidebarH();
    var cartH = _self.getCartHeight() - 50;

    if(cartH > sidebarArea) {
        return true;
    } else {
        return false;
    }

};

RC.sidebar.init = function(){
    var _self = this;
    _self.updateItemAmount();
    _self.addListeners();
    $(window).resize();
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

RC.sidebar.removeAffix = function(){
    var _self = this;

    $(window).off('.affix');
    RC.products.$cartContainer
    .removeClass("affix affix-top affix-bottom")
    .removeData("bs.affix");
};

RC.sidebar.addListeners = function(){
    var _self = this;

    $(window).on('resize.sidebar', RC.debounce(function(){
        var size = RC.getViewportName();
        if(size === "desktop") {
            if (!_self.cartTooHigh()) {
                _self.addAffix(); 
            }

            else {
                _self.removeAffix();
            }
        } 
        else if (size === "mobile") {
            _self.removeAffix();
        }

    },300));

    _self.$cartToggle.on('click.cartToggle', function() {
        $(this).toggleClass('active');
        _self.$sidebar.toggleClass('active');
        $('body').toggleClass('modal-open');
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


RC.products.updateButton = function(data_attr, new_val) {
    var _self = this;

    $( "[data-" + data_attr +"]" ).each( function() {
         
        var $el= $( this ),
            $cartArea = $el.find(RC.products.addToCartArea_CLASS),
            $add2CartButton = $('button.add2Cart',$cartArea);

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

RC.products.updateUi = function(id, data_attr, tu, new_val,tuVisible ){
    var _self = this;
    $( "[data-" + data_attr +"]" ).each( function() {

        var $el= $( this );

        if ($el.is('.product-item')) {
            var $cartArea = $el.find(RC.products.addToCartArea_CLASS);
                $add2CartButton = $('button.add2Cart',$cartArea),
                hasMulti = ($el.find('.dropdown-menu').length > 0) ? true : false;
            
            if (hasMulti) {
                var selectedTu = $el.find('.label-area > .price').data('tradingunit');
                $el.find('.label-area > .price').attr('data-currentamount',new_val);
                $el.find('.dropdown-menu li.hidden').find('a .price').attr('data-currentamount',new_val);
            }

            if (tuVisible) {
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
             }
        }
    });
};

RC.products.DataBinder = function( object_id ) {
    var pubSub = $({}),
        data_attr = "bind-" + object_id;

    pubSub.on("visible:change", function( evt,new_val) {
        
        RC.products.updateButton(data_attr, new_val);
    });
 
    pubSub.on("amount:change", function( evt, tu, new_val, ajaxUpdate ) {
   
        if (ajaxUpdate) { 
 
            RC.ajax.updateCart(object_id, tu, function(){
                var tuVisible = (RC.products.offers[object_id].isVisible(tu) ) ? true : false;
                RC.products.updateUi(object_id, data_attr, tu, new_val, tuVisible );
            });

        } else {
            RC.products.updateUi(object_id, data_attr, tu, new_val );
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
        var tu = {},
            $el  = $(this),
            id = $el.data('id'),
            offerid = $el.data('offerid'),
            variation = $el.data('variation'),
            visibletradingunit = $el.find('.label-area > .price').data('tradingunit');
           
            if ($el.find('.dropdown-menu').length > 0) {
               
                $el.find('.dropdown-menu li a .price').each(function(){
                    var $el = $(this),
                        id = $el.data('tradingunit'),
                        amount = $el.data('currentamount');

                    tu[id] = amount;
                });
            } 
            else {
                var amount = $el.find('.label-area > .price').data('currentamount');
                tu[visibletradingunit] = amount;
            }

            _self.offers[ id  ] = new _self.Item(id, offerid, variation, visibletradingunit,tu);
            _self.offers[ id  ].setVisible(visibletradingunit)
 
    });
 
};

RC.products.Item = function(id, offerid, variation, visibletradingunit, tradingunits ) {
    
    var binder = new RC.products.DataBinder(id);
     
    var item = {
        props : {
            offerid : offerid,
            variation : variation,
            visibletradingunit : visibletradingunit,
            tradingunits : tradingunits
        },
       
        get : function(attr_name){
            return this.props[attr_name];
        },

        setVisible : function(tu_id) {
            this.props.visibletradingunit = tu_id;
           var amount = this.getAmount(this.props.visibletradingunit);
            binder.trigger("visible:change", [amount] ); 
        },

        isVisible : function(tu_id) {
            if (this.getVisible()  === tu_id) {
                return true;
            }
            else {
                return false;
            }
        },

        getVisible : function() {
            return this.props.visibletradingunit;
        },

        setAmount : function(tu_id, val, ajaxUpdate) {
            this.props.tradingunits[ tu_id ] = val;
            binder.trigger("amount:change", [tu_id, val, ajaxUpdate] ); 
        },

        getAmount : function(tu_id) { 
            return this.props.tradingunits[ tu_id ];
        },


        increase : function(tu_id) {
            var newVal = this.getAmount(tu_id) + 1;
            this.setAmount(tu_id,newVal,true);
        },

        decrease : function(tu_id){
            var newVal = this.getAmount(tu_id) - 1;
            this.setAmount(tu_id,newVal,true);
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
    var id = $cont.data('id');
    var tu =  $cont.data('tradingunit');
    _self.offers[id].setAmount(tu, 0,true);
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
        var id = $cont.data('id');
        var tu = $cont.data('tradingunit');

        _self.offers[id].setAmount(tu, inputVal,true);
        _self.addLoader($cont);

    } else { 
        $el.val(_self.inputValCache);
    }
}

RC.products.handleinputFocus = function(e) {
    e.preventDefault();
    var _self = e.data.obj;
    var $el = $(this);
    $el.mouseup(function(e) { return false; });
    this.focus();
    $el.select();
    _self.inputValCache= $el.val();
}

RC.products.handleStepperClick = function(e) {
    e.preventDefault();
    var _self = e.data.obj;
    var $el = $(this);
    var $cont  =  $el.parents('.cart-item');
    var tu = parseInt($cont.data('tradingunit'),10);
    var id = parseInt($cont.data('id'),10);
    var $qty = $el.closest('.stepper').find('input.stepper-qty');
    var qtyVal = parseInt($qty.val(),10);
    var items =  qtyVal + (1 * $el.data('operation'));
    _self.offers[id].setAmount(tu, items,true);
    _self.addLoader($cont);
};

RC.products.handleDropdownSwitch = function(e){
    var _self = e.data.obj,
        $el = $(this),
        $anchor = $el.find('a'),
        amount = parseInt($anchor.find('.price').attr('data-currentamount'),10),
        selectedTu = parseInt($anchor.find('.price').attr('data-tradingunit'),10),
        id = parseInt($el.parents('.product-item').attr('data-id'),10);
    
    var newContent = $anchor.html();
        
    $el.closest( '.btn-group' )
        .find('.label-area').html(newContent)
        .end()
        .children( '.dropdown-toggle' ).dropdown( 'toggle' );
    $el.siblings().removeClass('hidden');
    $el.addClass('hidden');

    _self.offers[id].setVisible(selectedTu);
    
    return false;
};

RC.products.handleAdd2Cart = function(e){
    var _self = e.data.obj,
        $el = $(this),
        id = parseInt($el.parents('.product-item').attr('data-id'),10),
        item = _self.offers[id],
        visTu = item.getVisible();
 
    item.setAmount(visTu, 1,true);
 };
  
RC.products.handlePlus = function(e){
    var _self = e.data.obj,
        $el = $(this),
        id = $el.parents('.product-item').data('id'),
        item = _self.offers[id],
        visTu = item.getVisible();

    _self.offers[id].increase(visTu);
};

RC.products.handleMinus = function(e) {
    var _self = e.data.obj,
        $el = $(this),
        id = $el.parents('.product-item').data('id'),
        item = _self.offers[id],
        visTu = item.getVisible();

    _self.offers[id].decrease(visTu);
};

RC.ajax = {};

RC.ajax.updateCart = function(offerid, tu, callback, uid) {
    var _self  = this,
       /* visibletradingunit = RC.products.offers[offerid].get('visibletradingunit'),*/
        am = RC.products.offers[offerid].getAmount(tu);

    var item_data = {
        offerid : RC.products.offers[offerid].get('offerid'),
        variation : RC.products.offers[offerid].get('variation'),
        tradingunit : tu,
        amount : am,
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
             
            document.getElementById('cart-container').innerHTML = data;

           var size = RC.getViewportName();
           if(size === "desktop") {
               if (!RC.sidebar.cartTooHigh()) {
                   RC.sidebar.addAffix(); 
               }

               else {
                   RC.sidebar.removeAffix();
               }
           } 
           else if (size === "mobile") {
               RC.sidebar.removeAffix();
           }

            RC.sidebar.updateItemAmount();
            cb();
        }
    });
};
 
RC.init();

    $('#detailModal').on('shown.bs.modal', function() {
        $('#seller-image-carousel').carousel({
            interval: false
        });
    });

    $('#pickupModalToggle').on('click.pickupModal', function(e){
         e.preventDefault();
        $('#pickupModal').modal();
    });
 
 $('#pickupModal').on('shown.bs.modal', function() {
     var pathToTheme = Drupal.settings.basePath + "sites/all/themes/" + Drupal.settings.ajaxPageState.theme;
        var pickupIcon = pathToTheme + '/images/markers/pickup_icon.png';
        var infoWindow = new google.maps.InfoWindow;
        var latlng = [];
   
    var map = new google.maps.Map(document.getElementById("pickupMap"), {
            maxZoom: 15,
            mapTypeId: 'roadmap'
        });
    
      
        downloadUrl(Drupal.settings.basePath + 'rm-shop-spotxml/' + Drupal.settings.suid, function(data) {
        var xml = data.responseXML;
        var markers = xml.documentElement.getElementsByTagName("marker");
        
        for (var i = 0; i < markers.length; i++) {
             
            var address = markers[i].getAttribute("address");
            var id = markers[i].getAttribute("nid");
            
            var point = new google.maps.LatLng(
                parseFloat(markers[i].getAttribute("lat")),
                parseFloat(markers[i].getAttribute("lng"))
                );
            
            latlng.push(point)
                
            var html = "<b>" + address + "</b>";
            
            var marker = new google.maps.Marker({
                map: map,
                position: point,
                icon: pickupIcon,
                 
            });
            bindInfoWindow(marker, map, infoWindow, html);
        }
        
        var latlngbounds = new google.maps.LatLngBounds();
        for (var i = 0; i < latlng.length; i++) {
            latlngbounds.extend(latlng[i]);
        }
        map.setCenter(latlngbounds.getCenter());
        map.fitBounds(latlngbounds);
    }); 
  
});

 function downloadUrl(url,callback) {
    var request = window.ActiveXObject ?
        new ActiveXObject('Microsoft.XMLHTTP') :
        new XMLHttpRequest;

    request.onreadystatechange = function() {
        if (request.readyState == 4) {
            request.onreadystatechange = doNothing;
            callback(request, request.status);
        }
    };

    request.open('GET', url, true);
    request.send(null);
}

function bindInfoWindow(marker, map, infoWindow, html) {
    google.maps.event.addListener(marker, 'click', function() {
        infoWindow.setContent(html);
        infoWindow.open(map, marker);
    });
}


function doNothing() {
    
 }

 
 
   


                
});
