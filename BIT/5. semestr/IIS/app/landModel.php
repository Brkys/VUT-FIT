<?php

namespace App;

use Illuminate\Database\Eloquent\Model;

class landModel extends Model
{
    /**
     * tabulka spojena s modelem
     */
    protected $table = 'Uzemi';
    protected $primaryKey = 'ID_Uzemi';
    public $timestamps = false;
}