<?php
$mail = array(
  'subject' => 'Request for account deletion',
  'body'    => <<<EOBODY
[user:name] sent a request for deletion of his account.
To proceed, visit this page:
[user:edit-url]

His reasons:
[cancel-request-comment]
EOBODY
  ,
);

