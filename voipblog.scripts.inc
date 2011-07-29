<?php


/**
 * Implementation of hook_voipscript_get_script_names()
 */
function voipblog_voipscript_get_script_names() {
  $script_names[] = 'voipblog_welcome';
  $script_names[] = 'voipblog_record_options';
  $script_names[] = 'voipblog_record_story';
  return $script_names;
}

/**
 * Implementation of hook_voipscript_load_script()
 */
function voipblog_voipscript_load_script($script_name, $options=NULL) {
watchdog('voipblog', "(load_script) name: $script_name, options: ".print_r($options, true));

  require(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'voipblog.prompts.inc');

  $script = NULL;
  switch($script_name) {
    default:
      break;

    case 'voipblog_welcome':
      $options['language'] = 'English';

      $script = new VoipScript('voipblog_welcome');
      $script->addSetVoice('woman');

      $script->addSay($p_welcome_msg);

      $script->addLabel('script_options');
      $input_options = array(
        '1' => 'record_voipblog',
        '2' => 'listen_to_voipblogs',
        '3' => 'learn_more',
        'i' => 'invalid_option',
        't' => 'invalid_option'
      );
      $script->addRunIvrMenu($p_main_menu, $input_options);
      $script->addGoto('%ivr_option_selected');

      $script->addLabel('record_voipblog');
      $script->addGosub('voipblog_record_options', $options);
      $script->addGoto('go_back');

      $script->addLabel('listen_to_voipblogs');
$script->addSay('This is where callers will be able to browse through existing audio blog entries.');
      $script->addGoto('script_options');

      $script->addLabel('learn_more');
      $script->addSay($p_system_description);
      $script->addGoto('script_options');

      $script->addLabel('invalid_option');
      $script->addGoto('go_back');

      $script->addLabel('go_back');
      $script->addSay($p_thank_you);
      $script->addHangup();

      break;

    case 'voipblog_record_options':
      $script = new VoipScript('voipblog_record_options');

      $script->addLabel('recording_menu');
      $prompt = $p_recording_menu;
      $input_options = array(
        '1' => 'past',
        '2' => 'present',
        '3' => 'future',
        '#' => 'go_back',
        'i' => 'invalid_option',
        't' => 'invalid_option'
      );
      $script->addRunIvrMenu($prompt, $input_options);
      $script->addGoto('%ivr_option_selected');

      $script->addLabel('past');
$script->addSay('About to record story about the past.');
      $options['story_type'] = 'past';
      $script->addGosub('voipblog_record_story', $options);
      $script->addGoto('go_back');

      $script->addLabel('present');
$script->addSay('About to record a story about the present.');
      $options['story_type'] = 'present';
      $script->addGosub('voipblog_record_story', $options);
      $script->addGoto('go_back');

      $script->addLabel('future');
$script->addSay('About to record a story about the future');
      $options['story_type'] = 'future';
      $script->addGosub('voipblog_record_story', $options);
      $script->addGoto('go_back');

      $script->addLabel('invalid_option');
      $script->addGoto('go_back');

      $script->addLabel('go_back');
      $script->addReturn();

      break;

    case 'voipblog_record_story':
      $script = new VoipScript('voipblog_record_story');

      $voipblog_type = $options['story_type'];

      $script->addLabel('start');
      $prompt = $p_recording_prompt;
      $timeout = 5;
      $end_key = '#';
      $max_length = VOIPBLOG_MAX_STORY_DURATION;
      $script->addRecord($prompt, $timeout, $end_key, $max_length);

      $script->addLabel('accept_menu');
      $prompt = $p_recording_confirmation; 
      $input_options = array(
        '1' => 'play_recording',
        '2' => 'accept_recording',
        '*' => 'start',
        '#' => 'go_back',
        'i' => 'invalid_option',
        't' => 'invalid_option'
      );
      $script->addRunIvrMenu($prompt, $input_options);
      $script->addGoto('%ivr_option_selected');

      $script->addLabel('invalid_option');
      $script->addGoto('go_back');

      $script->addLabel('play_recording');
      $script->addSay($p_recording_playback);
      $script->addSay('%recording_url');
      $script->addGoto('accept_menu');

      $script->addLabel('accept_recording');
watchdog('voipblog', "options 1: " . print_r($options, true));
      $options['node_type'] = 'memory';
      $options['audio_field_name'] = 'field_memory_audio';
      $options['voipblog_type'] = $voipblog_type;
watchdog('voipblog', "options 2: " . print_r($options, true));
      $serialized_options = serialize($options);
watchdog('voipblog', "serialized_options: $serialized_options");
      $script->addSet('callback_result',
        "^voipblog_recording_callback(%call_id, %recording_url, %recording_duration, '$serialized_options')");
      $script->addSay($p_story_accepted);
      $script->addGoto('go_back');

      $script->addLabel('go_back');
      $script->addReturn();
      break;
  }

  return $script;
}


