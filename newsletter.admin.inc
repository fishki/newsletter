<?php
// $Id:$
function newsletter_list_form() {
    $form['search'] = array(
        '#type' => 'fieldset',
        '#title' => t('Search'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#theme' => 'newsletter_list_search_form',
    );
    $title = isset($_GET['title']) ? trim($_GET['title']) : FALSE;
    $form['search']['title'] = array(
        '#type' => 'textfield',
        '#title' => t('Title'),
        '#default_value' => filter_xss($title, array()),
        '#size' => 60,
    );
    $date_from = isset($_GET['date_from']) ? trim($_GET['date_from']) : FALSE;
    $form['search']['date_from'] = array(
        '#id' => 'search_date_from',
        '#type' => 'textfield',
        '#title' => t('Date from'),
        '#default_value' => filter_xss($date_from, array()),
        '#size' => 20,
    );
    $date_to = isset($_GET['date_to']) ? trim($_GET['date_to']) : FALSE;
    $form['search']['date_to'] = array(
        '#id' => 'search_date_to',
        '#type' => 'textfield',
        '#title' => t('Date to'),
        '#default_value' => filter_xss($date_to, array()),
        '#size' => 20,
    );
    $form['search']['search_button'] = array(
        '#type' => 'submit',
        '#value' => t('Search'),
    );
    $header = array(
      'title' => array('data' => t('Title'), 'field' => 'n.title'),
      'sending_date' => array('data' => t('Sending date'), 'field' => 'nl.sending_date'),
      'actions' => array('data' => t('Actions'), ),
    );
    
    $query = db_select('node', 'n')->extend('PagerDefault');
    $query->leftJoin('newsletter_list', 'nl', 'n.nid = nl.nid');
    $query->condition('n.type', 'newsletter');
    if($title) $query->condition('n.title', '%' . db_like($title) . '%', 'LIKE');
    if($date_from) $query->condition('nl.sending_date', $date_from, '>=');
    if($date_to) $query->condition('nl.sending_date', $date_to, '<=');
    $count_query = clone $query;
    $count_query->addExpression('COUNT(n.nid)');
    $query->extend('PagerDefault')->extend('TableSort')
      ->fields('n', array('nid', 'title'))
      ->fields('nl', array('sending_date'))
      ->limit(50)
      ->orderByHeader($header)
      ->setCountQuery($count_query);
    $result = $query->execute();
    $options = array();
    foreach ($result as $node) {
      $options[$node->nid] = array(
        'title' => l($node->title, 'newsletter/manage/edit/' . $node->nid),
        'sending_date' =>  $node->sending_date,
        'actions' => '<div>' . l(t('Preview'), 'newsletter/template/' . $node->nid, array('attributes' => array('target' => '_blank'))) . '</div><div>Send</div>',
      );
    }
    $form['nodes'] = array(
      '#type' => 'tableselect',
      '#header' => $header,
      '#options' => $options,
      '#empty' => t('No letter available.'),
    );
    $form['pager'] = array('#markup' => theme('pager'));
  
 /*   
    while($data = db_fetch_object($result)) {
        $form['newsletter_title'][$data->nid] = array('#value' => l($data->title, 'newsletter/manage/edit/' . $data->nid));
        $form['newsletter_sending_date'][$data->nid] = array('#value' => (int)$data->sending_date ?  date('Y-m-d', $data->sending_date) : '');
        $form['actions_preview'][$data->nid] = array('#value' => l(t('Preview'), 'newsletter/template/' . $data->nid, array('attributes' => array('target' => '_blank'))));
        $form['actions_send'][$data->nid] = array('#value' => l(t('Send'), 'send_mailiing/' . $data->nid, array('attributes' => array('calss' => 'send_link'))));
    }
  */
  return $form;
}

function newsletter_list_form_submit($form, $form_state) {
    $get_search = array();
    if (trim($form_state['values']['title'])) $get_search['title'] = trim($form_state['values']['title']);
    if (trim($form_state['values']['date_from'])) $get_search['date_from'] = date('Y-m-d', strtotime($form_state['values']['date_from']));
    if (trim($form_state['values']['date_to'])) $get_search['date_to'] = date('Y-m-d', strtotime($form_state['values']['date_to']));
    $options['query'] = $get_search;
    drupal_goto("newsletter", $options);
}

function theme_newsletter_list_search_form($variables) {
  $elements = $variables['search'];
  drupal_add_css(drupal_get_path('module', 'newsletter') . '/css/newsletter.css');
  drupal_add_library('system', 'ui.datepicker');
  drupal_add_js("jQuery(document).ready(function(){
                  jQuery('#search_date_from').datepicker({dateFormat: 'yy-mm-dd'});
                  jQuery('#search_date_to').datepicker({dateFormat: 'yy-mm-dd'});
                })", 'inline');

    $output = '<table class="newsletter_search_table">
        <tr>
            <td colspan="2">' . drupal_render($elements['title']) . '</td>
        </tr>
        <tr>
            <td>' . drupal_render($elements['date_from']) . '</td><td>' . drupal_render($elements['date_to']) . '</td>
        </tr>
        <tr>
            <td colspan="2">' . drupal_render($elements['search_button']) . '</td>
        </tr>
    </table>';
    return $output . drupal_render_children($elements);
}

function newsletter_rpeview($nid = NULL) {
    //$node = node_load($nid);
    echo theme('page', array('node' => $nid));
}

function newsletter_list_user_form() {
  $form['search'] = array(
    '#type' => 'fieldset',
    '#title' => t('Search'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    //'#theme' => 'newsletter_list_user_search_form',
  );
  $mail = isset($_GET['mail']) ? trim($_GET['mail']) : FALSE;
  $form['search']['mail'] = array(
    '#type' => 'textfield',
    '#title' => t('E-mail'),
    '#default_value' => filter_xss($mail, array()),
    '#size' => 60,
  );
  $unsubscribed = isset($_GET['unsubscribed']) ? trim($_GET['unsubscribed']) : FALSE;
  $approved = isset($_GET['approved']) ? trim($_GET['approved']) : FALSE;
  
  $option = array(
    'unsubscribed' => t('Unsubscribed'),
    'approved' => t('Aproved'),
  );
  
  $def_values = array();
  if($unsubscribed) $def_values[] = 'unsubscribed';
  if($approved) $def_values[] = 'approved'; 
  
  $form['search']['flags'] = array(
        '#type' => 'checkboxes',
        '#options' => $option,
        '#default_value' => $def_values,
    );
  
    $form['search']['search_button'] = array(
        '#type' => 'submit',
        '#value' => t('Search'),
    );
    $header = array(
      'username' => array('data' => t('Username'), 'field' => 'u.name'),
      'approved' => array('data' => t('Approved'), 'field' => 'nul.approved'),
      'subscribed' => array('data' => t('Subscribed'), 'field' => 'nul.unsubscribed'),
    );
    $query = db_select('users', 'u')->extend('PagerDefault');
    $query->leftJoin('newsletter_user_list', 'nul', 'u.uid = nul.uid');
    $query->condition('u.status', '1');
    if($mail) $query->condition('u.mail', '%' . db_like($mail) . '%', 'LIKE');
    if($unsubscribed) $query->condition('nul.unsubscribed', '1');
    if($approved) $query->condition('nul.approved', '1');
    $count_query = clone $query;
    $count_query->addExpression('COUNT(u.uid)');
    $query->extend('TableSort')
      ->fields('u', array('uid', 'name', 'mail'))
      ->fields('nul', array('approved', 'unsubscribed'))
      ->limit(5)
      ->orderByHeader($header)
      ->setCountQuery($count_query);
    $result = $query->execute();
    $options = array();
    foreach ($result as $user) {
    $options[$user->uid] = array(
      'username' => l($user->mail, 'newsletter/users/' . $user->uid . '/subscribe'),
      'approved' =>  $user->approved ? 'yes' : 'no',
      'subscribed' =>  $user->unsubscribed ? 'yes' : 'no',
    );
  }
  $form['users'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No users available.'),
  );
  $form['pager'] = array('#markup' => theme('pager'));
  return $form;
}

function newsletter_list_user_form_submit($form, $form_state) {
    $get_search = array();
    if (trim($form_state['values']['mail'])) $get_search['mail'] = trim($form_state['values']['mail']);
    if ($form_state['values']['flags']['unsubscribed']) $get_search['unsubscribed'] = '1';
    if ($form_state['values']['flags']['approved']) $get_search['approved'] = '1';
    $options['query'] = $get_search;
    drupal_goto("newsletter/users", $options);
}

function newsletter_user_subscribe($user) {
  
    return drupal_get_form('newsletter_user_subscribe_form', $user);
}

function newsletter_user_subscribe_form($form, &$form_state, $user) {
  //pre($user);die();
    $form['subscribe'] = array(
        '#type' => 'checkbox',
        '#title' => t('Subscribe') . ' ' . $user->name . ' ' . t('for mailing'),
    );
    
    $form['uid'] = array(
        '#type' => 'value',
        '#value' => $user->uid,
    );
    
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Subscribe'),
    );
    
    return $form;
}

function newsletter_user_subscribe_form_submit($form, $form_state) {
    if($form_state['values']['subscribe']) {
        $user = user_load($form_state['values']['uid']);
        $subscribe_key = md5('subscribe' . time());
        $unsubscribe_key = md5('unsubscribe' . time());
        $result = db_query("SELECT COUNT(*) FROM {newsletter_user_list} WHERE uid = :uid", array(':uid' => $form_state['values']['uid']));
        //echo $form_state['values']['uid'];
        if($result) {
            $vars['approve_key'] = $subscribe_key;
            $vars['unsubscribe_key'] = $unsubscribe_key;
            $vars['uid'] = $form_state['values']['uid'];
            pre($vars);die();
            drupal_write_record('newsletter_user_list', $vars, 'uid');
        }
        else {
            $vars['approve_key'] = $subscribe_key;
            //$vars['uid'] = $form_state['values']['uid'];
            //drupal_write_record('newsletter_user_list', $vars);
            $nid = db_insert('newsletter_user_list')
            ->fields(array('uid' => $form_state['values']['uid'], 'approve_key' => $subscribe_key, 'unsubscribe_key' => $unsubscribe_key))
            ->execute();
            //pre($nid);die();
            //db_query("INSERT INTO {newsletter_user_list} (uid, approve_key, unsubscribe_key) VALUES (:uid, '%s')", array(':uid' => $form_state['values']['uid'], $subscribe_key, $unsubscribe_key));
        }
        $user_account['approve_key'] = $subscribe_key;
        $user_account['name'] = $user->name;
        $mail = newsletter_user_mail_notify('newsletter_approve_notification', (object)$user_account);
        if($mail) {
            drupal_set_message(t('User %username has been subscribed', array('%username' => $user->name)));
        }
    }
}