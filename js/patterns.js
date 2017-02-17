(function($) {
  Drupal.behaviors.sebd7tweaksPatterns = {
    attach: function(context, settings) {
      $('.sebd7tweaks-pattern[pattern!=""]', context).keyup(function() {
        var pattern = settings.sebd7tweaks.patterns[$(this).attr('pattern')];

        if (typeof pattern != 'undefined') {
          var filtered = this.value.replace(new RegExp(pattern, 'g'), '');

          if (filtered != this.value) {
            this.value = filtered;
          }
        }
      });
    }
  };
}) (jQuery);

