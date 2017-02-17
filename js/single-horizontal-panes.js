(function($) {
  Drupal.behaviors.sebd7tweaksSingleHorizontalPanes = {
    attach: function(context, settings) {
      $('.horizontal-tabs ul.horizontal-tabs-list', context).each(function() {
        if ($(this).children('li.horizontal-tab-button').length == 1) {
          $(this).remove();
        }
      });
    }
  };
}) (jQuery);

