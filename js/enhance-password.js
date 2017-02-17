(function($) {
  Drupal.behaviors.sebd7tweaksPassword = {
    attach: function(context, settings) {
      $('input.password-field.password-processed', context).once('sebd7tweaks', function() {
        var passwordInput       = $(this),
            innerWrapper        = $(this).parent(),
            outerWrapper        = $(this).parent().parent(),
            passwordMeter       = $('div.password-strength',  innerWrapper),
            confirmInput        = $('input.password-confirm', outerWrapper),
            confirmResult       = $('div.password-confirm',   outerWrapper),
            passwordDescription = $('div.password-suggestions', outerWrapper);

        confirmInput.keyup( function() { confirmResult.toggle(confirmInput.val()  != ''); }).after(confirmResult.hide());
        passwordInput.keyup(function() { passwordMeter.toggle(passwordInput.val() != ''); }).after(passwordMeter.hide())
          .blur(function() { passwordDescription.hide(); }).focus(function() {
            if ($('li:not(:empty)', passwordDescription).length > 0) {
              passwordDescription.show();
            }
          });
      });
    }
  };
}) (jQuery);


