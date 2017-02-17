<?php
function sebd7tweaks_API_user_field_privacy_declare_features() {
  return array('private_by_default' => t('Check "private" checkboxes by default on user registration form.'));
}

function sebd7tweaks_API_user_field_privacy_declare_additions() {
  $options = array('query' => drupal_get_destination(), 'html' => TRUE);
  $items   = array(t('Public'), t('Private'));

  foreach ($items as $key => &$item) {
    $item = l(t('Make all fields (with private choice) %state for existing users', array('%state' => $item)),
              'admin/config/sebd7tweaks/user-field-privacy/'. (int) $key, $options);
  }

  return array('links' => array('#theme' => 'item_list', '#items' => $items));
}

####################
# Helper functions #
####################

/**
 * Helper function: Apply to all existing users fields which support privacy choice a default status
 *
 * BEWARE: This is not supposed to be done on site with lots of users and/or private fields, because...
 * This perform one merge by field and by user, but unfortunately, db_merge can't work with more than a row!
 * This is not supported by Drupal 7 and Drupal 8 will not implement this either :(
 */
function _sebd7tweaks_API_user_field_privacy_apply_state($state = 1) {
  // Anonymous user (uid = 0) should not be included in users list
  $uids = db_select('users', 'u')->condition('u.uid', 0, '<>')->fields('u', array('uid'))->execute();
  $fids  = array();
  // Get IDs list of fields which can be set private
  foreach (field_info_instances('user', 'user') as $instance) {
    if ($instance['settings']['user_field_privacy']) {
      $fids[] = $instance['field_id'];
    }
  }
  // Now put them private by default for all users
  while ($uid = $uids->fetchField()) {
    foreach ($fids as $fid) {
      db_merge('user_field_privacy_value')
        // Keys on which unicity will be checked
        ->key(array('uid' => $uid, 'fid' => $fid))
        // Fields values to insert if not already defined
        ->fields(array('uid' => $uid, 'fid' => $fid, 'private' => $state))
        // Better way I've found to let the row intact (updateFields method doesn't accept an empty array and without, the field is updated)
        ->expression('private', 'private')
        // If you can't understand what the following line does... You're not a developper :p
        ->execute();
    }
  }
  // Redirect to the previous page
  drupal_goto();
}

