<?php
function sebd7tweaks_API_logintoboggan_declare_features() {
  return array(
    'denied_page_blocks'  => t('Do not hide blocks on denied pages.'),
    'enhance_double_mail' => t('Dynamically show the e-mail confirmation (requiert %module, with option %option activated)',
                              array('%module' => 'logintoboggan', '%option' => t('Use two e-mail fields on registration form'))),
  );
}

