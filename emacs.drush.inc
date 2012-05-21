<?php

/**
 * @file
 * Various useful drush commands and extensions for Emacs users.
 */

define('DRUSH_EMACS_ETAGS_EXTENSIONS', 'module,inc,php,profile,install');

/**
 * Implements hook_drush_command().
 */
function emacs_drush_command() {
  $items = array();

  $items['etags'] = array(
    'description' => dt('Make/update Emacs TAGS file'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_ROOT,
    'examples' => array(
      'drush etags' => dt('Make/update an Emacs TAGS file in DRUPAL_ROOT.'),
    ),
    'options' => array(
      'emacs-etags-extensions' => dt('On which file extensions to run etags. Defaults to "@extensions".', array('@extensions' => DRUSH_EMACS_ETAGS_EXTENSIONS)),
      'emacs-etags-etags-program' => dt('Name of the etags executable. Include path if it cannot be found in $PATH.'),
      'emacs-etags-find-program' => dt('Name of the find executable. Include path if it cannot be found in $PATH.'),
    ),
  );
  $items['gtags'] = array(
    'description' => dt('Make/update GNU GLOBAL files'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_ROOT,
    'examples' => array(
      'drush gtags' => dt('Make/update GNU GLOBAL files in DRUPAL_ROOT.'),
    ),
    'options' => array(
      'emacs-gtags-gtags-program' => dt('Name of the gtags executable. Include path if it cannot be found in $PATH.'),
      'emacs-gtags-gtags-options' => dt('Options to add to gtags. Defaults to none.'),
    ),
  );

  return $items;
}

/**
 * Make/update an Emacs TAGS file in DRUPAL_ROOT.
 */
function drush_emacs_etags() {
  $extensions = explode(',', drush_get_option('emacs-etags-extensions', DRUSH_EMACS_ETAGS_EXTENSIONS));
  $etags = drush_get_option('emacs-etags-etags-program', 'etags');
  $find = drush_get_option('emacs-etags-find-program', 'find');

  $etags_command = "$find . -type f -name \*." . implode(' -or -name \*.', $extensions) . " | $etags --language=php -";

  chdir(drush_locate_root());

  drush_log('Creating/updating Emacs TAGS file...');
  system($etags_command, $return);

  if ($return) {
    return drush_set_error('DRUSH_EMACS', dt('Problem executing "@command".', array('@command' => $etags_command)));
  }

  drush_log('Creating/updating Emacs TAGS file...', 'success');
}

/**
 * Make/update GNU GLOBAL files in DRUPAL_ROOT.
 */
function drush_emacs_gtags() {
  $gtags = drush_get_option('emacs-gtags-gtags-program', 'gtags');
  $options = drush_get_option('emacs-gtags-gtags-options', '');

  $gtags_command = rtrim("$gtags $options");

  chdir(drush_locate_root());

  drush_log('Creating/updating GNU GLOBAL files...');
  system($gtags_command, $return);

  if ($return) {
    return drush_set_error('DRUSH_EMACS', dt('Problem executing "@command".', array('@command' => $gtags_command)));
  }

  drush_log('Creating/updating GNU GLOBAL files...', 'success');
}


/**
 * Implements hook_drush_pm_post_download().
 *
 * After downloading a module we run etags and/or gtags if we think etags/gtags
 * is in use (we think it is in use if a TAGS/GTAGS file is present in
 * DRUPAL_ROOT).
 *
 * You can disable this by setting either:
 *
 *  $options['emacs-pm-post-download-etags'] = FALSE;
 *  $options['emacs-pm-post-download-gtags'] = FALSE;
 *
 * in your drushrc.php or in your @site-alias.
 */
function emacs_drush_pm_post_download($project, $release) {
  if (drush_get_option('emacs-pm-post-download-etags', TRUE) && file_exists(drush_locate_root() . "/TAGS")) {
    drush_emacs_etags();
  }
  if (drush_get_option('emacs-pm-post-download-gtags', TRUE) && file_exists(drush_locate_root() . "/GTAGS")) {
    drush_emacs_gtags();
  }
}
