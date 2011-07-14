<div class="grid_3">
	<div class="box menubox">
		<h2><a href="#" id="toggle-admin-actions">Actions</a></h2>
		<div class="inbox">
			<div class="block" id="admin-actions">
				<h5><?php echo __('Imported Translation Files', true); ?></h5>
				<ul class="menu">				<li><?php echo $this->Html->link(sprintf(__('List %s', true), __('Imported Translation Files', true)), array('action' => 'index')); ?> </li>
				</ul>				<h5><?php echo __('Languages', true); ?></h5>
				<ul class="menu">
				<li><?php echo $this->Html->link(sprintf(__('List %s', true), __('Languages', true)), array('controller' => 'languages', 'action' => 'index')); ?> </li>
				</ul>
				<h5><?php echo __('Raw Files', true); ?></h5>
				<ul class="menu">
				<li><?php echo $this->Html->link(sprintf(__('List %s', true), __('Raw Files', true)), array('controller' => 'raw_files', 'action' => 'index')); ?> </li>
				</ul>
				<h5><?php echo __('File Identifiers', true); ?></h5>
				<ul class="menu">
				<li><?php echo $this->Html->link(sprintf(__('List %s', true), __('File Identifiers', true)), array('controller' => 'file_identifiers', 'action' => 'index')); ?> </li>
				</ul>
			</div>
		</div>
	</div>
</div>

<div class="grid_13">
	<h2 id="page-heading"><?php __('Imported Translation Files');?></h2>
	<table cellpadding="0" cellspacing="0">		<?php $tableHeaders = $html->tableHeaders(array($paginator->sort('id'),$paginator->sort('language_id'),$paginator->sort('filename'),$paginator->sort('merged'),$paginator->sort('file_last_modified_date'),$paginator->sort('created'),$paginator->sort('modified'),__('Actions', true),));
		echo '<thead>'.$tableHeaders.'</thead>'; ?>

		<?php
		$i = 0;
		foreach ($importedTranslationFiles as $importedTranslationFile):
			$class = null;
			if ($i++ % 2 == 0) {
				$class = ' class="altrow"';
			}
		?>
	<tr<?php echo $class;?>>
		<td><?php echo $importedTranslationFile['ImportedTranslationFile']['id']; ?></td>
		<td>
			<?php echo $this->Html->link($importedTranslationFile['Language']['name'], array('controller' => 'languages', 'action' => 'view', $importedTranslationFile['Language']['id'])); ?>
		</td>
		<td><?php echo $importedTranslationFile['ImportedTranslationFile']['filename']; ?></td>
		<td><?php echo $importedTranslationFile['ImportedTranslationFile']['merged']; ?></td>
		<td><?php echo $importedTranslationFile['ImportedTranslationFile']['file_last_modified_date']; ?></td>
		<td><?php echo $importedTranslationFile['ImportedTranslationFile']['created']; ?></td>
		<td><?php echo $importedTranslationFile['ImportedTranslationFile']['modified']; ?></td>
		<td class="actions">
			<?php echo $this->Html->link(__('View', true), array('action' => 'view', $importedTranslationFile['ImportedTranslationFile']['id'])); ?>
		</td>
	</tr>
	<?php endforeach; ?>
	<?php echo '<tfoot class=\'dark\'>'.$tableHeaders.'</tfoot>'; ?>
	</table>

	
	<p>
	<?php
	echo $this->Paginator->counter(array(
	'format' => __('Page %page% of %pages%, showing %current% records out of %count% total, starting on record %start%, ending on %end%', true)
	));
	?>	</p>

	<div class="paging">
		<?php echo $this->Paginator->prev('<< '.__('previous', true), array(), null, array('class'=>'disabled'));?>
		| <?php echo $this->Paginator->numbers();?> |
		<?php echo $this->Paginator->next(__('next', true).' >>', array(), null, array('class' => 'disabled'));?>
	</div>
</div>
<div class="clear"></div>
