<?php
function sebd7tweaks_API_follow_declare_features() {
  return array(
    'hide_personal'  => t('Hide personal follow tab for admin (usefull if follow module is only activated to display site social links)'),
    'simplify_block' => t('Hide advanced options in follow site block configuration form'),
    'reorder_links'  => t('Reorder disabled links alphabetically'),
  );
}

