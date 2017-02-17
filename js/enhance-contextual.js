(function($) {
  Drupal.behaviors.sebd7tweaksContextual = {
    attach: function(context, settings) {
      $('div.contextual-links-wrapper>a.contextual-links-trigger', context).once('sebd7tweaks', function () {
        var links = $(this).siblings('ul.contextual-links').find('>li>a');

        if (links.length == 1) {
          $(this).addClass('single-option').click(function() { links[0].click(); });
        }
      });
    }
  };
}) (jQuery);

