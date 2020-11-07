<?php

namespace App;

use Illuminate\Database\Eloquent\Model;

class Familie extends Model
{
    /**
     * tabulka spojena s modelem
     */
    protected $table = 'Familie';
    public $timestamps = false;
    protected $primaryKey = 'ID_Familie';
}