<?php

namespace App;

use Illuminate\Database\Eloquent\Model;

class AlianceRequest extends Model
{
    /**
     * tabulka spojena s modelem
     */
    protected $table = 'PozvankaDoAliance';
    public $timestamps = false;
}