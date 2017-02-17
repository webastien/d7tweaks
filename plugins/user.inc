<?php
#################
# API functions #
#################

function sebd7tweaks_API_user_declare_features() {
  return array(
    'theme_account_title' => t('Use the theme username function to display accounts page title'),
    'no_auto_register'    => t('Add "autocomplete=off" to the fields of the registration form'),
    'hide_unneeded_mail'  => t('Hide unneeded mails settings in our custom "site basic informations" page'),
    'cancel_account_link' => t('Add a "cancel account" link to edit account form'),
    'enhance_pass_fields' => t('Dynamically show the password and security notification + active e-mail field on user creation / modification'),
    'hide_access_filter'  => t('Hide the permissions filter on users list for people who has not the permission %permission',
                              array('%permission' => t('Administer permissions'))),
  );
}

function sebd7tweaks_API_user_get_by_permission($permission, $fields = array(), $load = FALSE) {
  $users  = array();
  $fields = is_string($fields)? array($fields): (is_array($fields)? $fields: array());
  $query  = db_select('users', 'u')->fields('u', array('uid'));

  $query->join('users_roles',     'ur', 'ur.uid = u.uid');
  $query->join('role_permission', 'rp', 'rp.rid = ur.rid');
  $query->condition('rp.permission', $permission);

  foreach ($query->execute()->fetchAll() as $account) {
    $users[$account->uid] = $account->uid;
  }

  if ($load || count($fields) > 0) {
    $users = user_load_multiple($users);
  }

  if (count($fields) > 0) {
    foreach ($users as &$account) {
      if (count($fields) == 1) {
        $account = $account->$fields[0];
      }
      else {
        $row = array();

        foreach ($fields as $field) {
          $row[$field] = $account->$field;
        }

        $account = $row;
      }
    }
  }

  return $users;
}

