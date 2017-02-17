(function($) {
  Drupal.behaviors.sebd7tweaksContactBlock = {
    attach: function(context, settings) {
      var fields = $('#contact-site-form input[placeholder!=""],#contact-site-form textarea[placeholder!=""]', context);

      if (fields.length > 0 && 'placeholder' in document.getElementById(fields[0].id)) {
        fields.each(function() { $(this).closest('.form-item').find('label').addClass('element-invisible'); });
      }
    }
  };
}) (jQuery);

