<?php

/**
 * Main page for the TARDIS settings. 
 */
function tardis_settings() {
  $tardis_admin_items = array(
    t('<a href="@url">TARDIS page settings</a><br />Settings for the TARDIS page', array('@url' => url('admin/structure/tardis/page'))),
    t('<a href="@url">TARDIS block settings</a><br />Settings for the TARDIS block', array('@url' => url('admin/structure/tardis/block'))),
  );

  $content['tardis_settings'] = array(
    '#theme' => 'item_list',
    '#type' => 'ul',
    '#items' => $tardis_admin_items,
  );

  return render($content);
}

/**
 * System settings form for the TARDIS block of recent months. 
 */
function tardis_block_settings($form, &$form_state) {
  $form['tardis_block_title'] = array(
    '#type' => 'textfield',
    '#title' => t('TARDIS block title'),
    '#description' => t('Give your TARDIS block a title'),
    '#default_value' => variable_get('tardis_block_title', 'Recent nodes'),
  );

  // How far back is the TARDIS supposed to go and fetch results?
  $form['tardis_block_past_months'] = array(
    '#type' => 'textfield',
    '#size' => 2,
    '#maxlength' => 2,
    '#title' => t('How far back?'),
    '#field_suffix' => t('months'),
    '#description' => t('Choose how many months you would like displayed on the TARDIS block.'),
    '#default_value' => variable_get('tardis_block_past_months', 12),
  );

  // Define a certain point beyond which the TARDIS shouldn't look. 
  $form['tardis_block_stop_year'] = array(
    '#type' => 'textfield',
    '#size' => 4,
    '#maxlength' => 4,
    '#title' => t('Stop looking at the year'),
    '#description' => t("For increased performance, define a year (in the format YYYY) beyond which the TARDIS should stop looking.
    <br />If you don't, the TARDIS will keep looking until it reaches year zero, which means going over the db a LOT of times."),
    '#default_value' => variable_get('tardis_block_stop_year', 2001),
  );

  // Option to nest months within years. 
  $form['tardis_block_link_nesting_months_years'] = array(
    '#type' => 'checkbox',
    '#title' => t('Month link nesting'),
    '#description' => t("Should months be nested under years?"),
    '#default_value' => variable_get('tardis_block_link_nesting_months_years', TRUE),
  );

  // Lets users choose how year links will be styled. 
  $form['tardis_block_year_link'] = array(
    '#type' => 'radios',
    '#title' => t('Year link style'),
    '#description' => t("How should TARDIS render years?
      <ul>
        <li><em>Accordion: </em> year links will render as collapsible blocks with nested months.</li>
        <li><em>Normal link: </em> year links will link to the TARDIS page, just like month links do.</li>
        <li><em>No link: </em> year links will be displayed, but without links.</li>
      </ul>
    "),
    '#options' => array(
      'accordion' => t('Accordion'),
      'normal' => t('Normal link'),
      'no_link' => t('No link'),
    ),
    '#default_value' => variable_get('tardis_block_year_link', 'normal'),
    '#states' => array(
      'visible' => array(
        ':input[name="tardis_block_link_nesting_months_years"]' => array('checked' => TRUE),
      ),
    ),
  );

  // Lets users choose how month links will be styled. 
  $form['tardis_block_month_link'] = array(
    '#type' => 'radios',
    '#title' => t('Month link style'),
    '#description' => t("Choose whether you want months displayed as numbers or names."),
    '#options' => array(
      'names' => t('names'),
      'numbers' => t('numbers'),
    ),
    '#default_value' => variable_get('tardis_block_month_link', 'names'),
  );

  // Allows users to override the standard TARDIS link 
  $form['tardis_block_custom_link'] = array(
    '#type' => 'textfield',
    '#title' => t('Custom link'),
    '#description' => t("Choose a custom address for the TARDIS links (if you already have a view, for example). <br />
    No beginning or trailing slashes, please.<br />
    Please note: your custom address must take arguments in the form YYYY/MM just like the TARDIS page does. <br />
    If you don't need a custom link, just leave this field blank."),
    '#default_value' => variable_get('tardis_block_custom_link', ''),
  );

  $form['#validate'][] = 'tardis_block_settings_validate';

  return(system_settings_form($form));
}

/**
 * System settings form for the TARDIS page of recently added nodes. 
 */
function tardis_page_settings($form, &$form_state) {
  // Page title
  $form['tardis_page_title'] = array(
    '#type' => 'textfield',
    '#title' => t('TARDIS page title'),
    '#description' => t('Give your TARDIS page a title.<br />
      <em>Please note: </em>changing this setting requires that you clear your cache.'),
    '#default_value' => variable_get('tardis_page_title', 'Recent nodes'),
  );

  // Option to append a date to the page title
  $form['tardis_page_title_date'] = array(
    '#type' => 'radios',
    '#title' => t('Append date to page title'),
    '#description' => t('Should TARDIS append the year and/or month being viewed to the page title?'),
    '#options' => array(
      'years_month_names' => t('Years and month names'),
      'years_month_numbers' => t('Years and month numbers'),
      'no_date' => t('No date on page title'),
    ),
    '#default_value' => variable_get('tardis_page_title_date', 'no_date'),
  );

  // Where is the TARDIS parked? ;)
  $form['tardis_page_address'] = array(
    '#type' => 'textfield',
    '#title' => t('TARDIS page address'),
    '#description' => t('Choose an address for your TADIS page. <br />
      For example: <em>recent/nodes</em> leads to <em>http://yoursite.com/recent/nodes</em>. <br />
      No beginning or trailing slashes, please. <br />
      <em>Please note: </em>changing this setting requires that you clear your cache.'),
    '#default_value' => variable_get('tardis_page_address', 'recent/nodes'),
  );

  // Get all available node types and build an associative array 
  // with the format machine_name => 'human-readable name' 
  $node_types = node_type_get_types();
  foreach ($node_types as $node_type) {
      $options[$node_type->type] = $node_type->name;
  }
  // Now ask site admins which nodes they want displayed in the TARDIS
  // page and block 
  $form['tardis_page_node_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Node types'),
    '#description' => t('Which node types would you like displayed?'),
    '#options' => $options,
    '#default_value' => variable_get('tardis_page_node_types', array('page')),
  );

  $form['tardis_page_nodes_per_page'] = array(
    '#type' => 'textfield',
    '#size' => 2,
    '#maxlength' => 2,
    '#title' => t('Nodes per page'),
    '#field_suffix' => t('nodes on each page'),
    '#description' => t('Results are paged. How many nodes would you like to show on each page?'),
    '#default_value' => variable_get('tardis_page_nodes_per_page', 10),
  );

  $form['#validate'][] = 'tardis_page_settings_validate';

  return(system_settings_form($form));
}

function tardis_block_settings_validate($form, &$form_state) {
  // Define the validation strings. 
  $tardis_block_stop_year = $form_state['values']['tardis_block_stop_year'];
  $tardis_block_past_months = $form_state['values']['tardis_block_past_months'];

  // The "Stop looking at the year" field should obviously be a year. 
  if (!preg_match('/^[0-9]{4}$/', $tardis_block_stop_year)) {
    form_set_error ('tardis_block_stop_year', 'Please input a valid year with four digits.');
  }

  // And the "How far back?" one should be a two-digit number. 
  if (!preg_match('/^[0-9]{2}$/', $tardis_block_past_months) || $tardis_block_past_months <= 1) {
    form_set_error ('tardis_block_past_months', 'Please input a valid number of months between 1 and 99.');
  }
}

function tardis_page_settings_validate($form, &$form_state) {
  // Define the validation strings. 
  $tardis_page_address = $form_state['values']['tardis_page_address'];
  $tardis_page_title = $form_state['values']['tardis_page_title'];
  $tardis_page_nodes_per_page = $form_state['values']['tardis_page_nodes_per_page'];

  // Did the page address or title change? If so, we need to rebuild the menu.
  if ($tardis_page_address != variable_get('tardis_page_address') || $tardis_page_title != variable_get('tardis_page_title')) {
    drupal_set_message(t("Don't forget to <a href='/admin/config/development/performance'>clear your cache.</a>"));
  }

  // The "Nodes per page" field should be a two-digit number. 
  if (!preg_match('/^[0-9]{2}$/', $tardis_page_nodes_per_page) || $tardis_page_nodes_per_page <= 1) {
    form_set_error ('tardis_page_nodes_per_page', 'Please input a valid number of nodes per page between 1 and 99.');
  }
}