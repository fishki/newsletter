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
 /////%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%   
    $options = array();
    foreach ($result as $node) {
    $options[$node->nid] = array(
      'title' => $node->title,
      'sending_date' =>  $node->sending_date,
      'actions' => '<div>Preview</div><div>Send</div>',
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
    //pre($form_state);die();
    $get_search = array();
    if (trim($form_state['values']['title'])) $get_search['title'] = trim($form_state['values']['title']);
    if (trim($form_state['values']['date_from'])) $get_search['date_from'] = strtotime($form_state['values']['date_from']);
    if (trim($form_state['values']['date_to'])) $get_search['date_to'] = strtotime($form_state['values']['date_to']);
    $options['query'] = $get_search;
    drupal_goto("newsletter", $options);
}


function theme_newsletter_list_search_form($variables) {
  $elements = $variables['search'];
  drupal_add_css(drupal_get_path('module', 'newsletter') . '/css/newsletter.css');
  //drupal_add_js(drupal_get_path('module', 'newsletter') . '/js/jquery1.4.3.js');
  drupal_add_js(drupal_get_path('module', 'newsletter') . '/js/jquery-ui-1.8.6.datepicker.min.js');
  drupal_add_css(drupal_get_path('module', 'newsletter') . '/css/ui-lightness/jquery-ui-1.8.6.datepicker.css');
  drupal_add_js("
        $(document).ready(function(){
            $('#search_date_from').datepicker({dateFormat: 'yy-mm-dd'});
            $('#search_date_to').datepicker({dateFormat: 'yy-mm-dd'});
            
            
        });
    ", 'inline');

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
    //pre($elements);die();
    return $output . drupal_render_children($elements);
}


