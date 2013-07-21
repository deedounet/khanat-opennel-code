{block name=content}
<div class="row-fluid sortable ui-sortable">
    <div class="box span9">
        <div class="box-header well" data-original-title="">
            <h2><i class="icon-tag"></i> Ticket Queue {$queue_view}</h2>
            <div class="box-icon">
                <a href="#" class="btn btn-minimize btn-round"><i class="icon-chevron-up"></i></a>
                <a href="#" class="btn btn-close btn-round"><i class="icon-remove"></i></a>
            </div>
        </div>
        <div class="box-content">
            <div class="row-fluid">
                <legend>Tickets</legend>
		
		{if isset($ACTION_RESULT) and $ACTION_RESULT eq "SUCCESS_ASSIGNED"}
		<div class="alert alert-success">
			{$success_assigned}
		</div>
		{else if isset($ACTION_RESULT) and $ACTION_RESULT eq "SUCCESS_UNASSIGNED"}
		<div class="alert alert-success">
			{$success_unassigned}
		</div>
		{else if isset($ACTION_RESULT) and $ACTION_RESULT eq "TICKET_NOT_EXISTING"}
		<div class="alert alert-error">
			{$ticket_not_existing}
		</div>
		{else if isset($ACTION_RESULT) and $ACTION_RESULT eq "ALREADY_ASSIGNED"}
		<div class="alert alert-error">
			{$already_assigned}
		</div>
		{else if isset($ACTION_RESULT) and $ACTION_RESULT eq "NOT_ASSIGNED"}
		<div class="alert alert-error">
			{$not_assigned}
		</div>
		{/if}
		
		<table class="table table-striped table-bordered bootstrap-datatable datatable">
		    <thead>
			    <tr>
				    <th>ID</th>
				    <th>Title</th>
				    <th>Assigned</th>
				    <th>Timestamp</th>
				    <th>Category</th>
				    <th>Status</th>
				    <th>Actions</th>
			    </tr>
		    </thead>   
		    <tbody>
			  {foreach from=$tickets item=ticket}
			  <tr>
				<td>{$ticket.tId}</td>
				<td><a href ="index.php?page=show_ticket&id={$ticket.tId}">{$ticket.title}</a></td>
				<td>{if $ticket.assignedText neq ""} <a href="index.php?page=show_user&id={$ticket.assigned}">{$ticket.assignedText} {else}<i> {$not_assigned}</i> {/if}</td>
				<td class="center"><i>{$ticket.timestamp}</i></td>
				<td class="center">{$ticket.category}</td>
				<td class="center"><span class="label {if $ticket.status eq 0}label-success{else if $ticket.status eq 1}label-warning{else if $ticket.status eq 2}label-important{/if}">{if $ticket.status eq 0} <i class="icon-exclamation-sign icon-white"></i>{/if} {$ticket.statusText}</span></td>  
				<td>
				    {if $ticket.assigned eq 0}
					<form id="assign_ticket" class="form-vertical" method="post" action="" style="margin:0px 0px 0px;">
					    <input type="hidden" name="ticket_id" value="{$ticket.tId}">
					    <input type="hidden" name="action" value="assignTicket">
					    <button type="submit" class="btn btn-primary" ><i class="icon-flag icon-white"></i> Assign Ticket</button>
					</form>
				    {else if $ticket.assigned eq 1}
					<form id="assign_ticket" class="form-vertical" method="post" action="" style="margin:0px 0px 0px;">
					    <input type="hidden" name="ticket_id" value="{$ticket.tId}">
					    <input type="hidden" name="action" value="unAssignTicket">
					    <button type="submit" class="btn btn-warning" ><i class="icon-remove icon-white"></i> Remove Assign</button>
					</form>
				    {/if}
				</td>
			  </tr>
			  {/foreach}
	  
		    </tbody>
	    </table>            
	    </div>
	</div>
    </div><!--/span-->
    
    <div class="box span3">
        <div class="box-header well" data-original-title="">
            <h2><i class="icon-th"></i>Actions</h2>
            <div class="box-icon">
                <a href="#" class="btn btn-minimize btn-round"><i class="icon-chevron-up"></i></a>
                <a href="#" class="btn btn-close btn-round"><i class="icon-remove"></i></a>
            </div>
        </div>
        <div class="box-content">
            <div class="row-fluid">
		<div class="btn-group">
                <button class="btn btn-primary btn-large dropdown-toggle" data-toggle="dropdown">Actions<span class="caret"></span></button>
                <ul class="dropdown-menu">
		    <li class="divider"></li>
		    <li><a href="index.php?page=show_queue&get=all_open">All open tickets</a></li>
		    <li><a href="index.php?page=show_queue&get=archive">Ticket Archive</a></li>
		    <li><a href="index.php?page=show_queue&get=not_assigned">Not Assigned Tickets</a></li>
		    <li class="divider"></li>
                </ul>
              </div>
            </div>                   
        </div>
    </div><!--/span-->
</div><!--/row-->
{/block}
	
