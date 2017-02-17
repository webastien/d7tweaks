<?php
function sebd7tweaks_API_book_declare_features() {
  return array(
    'required_bid'  => t('Make book ID required for types used as book page.'),
    'new_to_end'    => t('Put new book pages to the end of its parent.'),
    'tweak_outline' => t('Replace tab "outline" page by the book structure page.'),
    'ordered_books' => t('Allow the ability to order books (and not only their pages).'),
  );
}

function sebd7tweaks_API_book_get_books() {
  $books = &drupal_static(__FUNCTION__);

  if (!isset($books)) {
    $books = book_get_books();

    if (sebd7tweaks_feature('book.ordered_books') && ($default = variable_get('sebd7tweaks_books_weights', FALSE))) {
      foreach (array_keys($books) as $bid) {
        if (!isset($default[$bid])) {
          $default[$bid] = 0;
        }
      }

      asort($default);
      $ordered = array();

      foreach (array_keys($default) as $bid) {
        if (isset($books[$bid])) {
          $ordered[$bid] = $books[$bid];
        }
      }

      $books = $ordered;
    }
  }

  return $books;
}

