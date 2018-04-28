/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: PdmOidDefs.h /main/7 1996/08/12 18:42:47 cde-hp $ */
/* This is an automatically-generated file. Do not edit. */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

typedef enum {
    pdmoid_none,
    pdmoid_unspecified,
    pdmoid_att_descriptor,
    pdmoid_att_content_orientation,
    pdmoid_att_copy_count,
    pdmoid_att_default_printer_resolution,
    pdmoid_att_default_input_tray,
    pdmoid_att_default_medium,
    pdmoid_att_document_format,
    pdmoid_att_plex,
    pdmoid_att_xp_listfonts_modes,
    pdmoid_att_job_name,
    pdmoid_att_job_owner,
    pdmoid_att_notification_profile,
    pdmoid_att_xp_setup_state,
    pdmoid_att_xp_spooler_command_options,
    pdmoid_att_content_orientations_supported,
    pdmoid_att_document_formats_supported,
    pdmoid_att_dt_pdm_command,
    pdmoid_att_input_trays_medium,
    pdmoid_att_medium_source_sizes_supported,
    pdmoid_att_plexes_supported,
    pdmoid_att_printer_model,
    pdmoid_att_printer_name,
    pdmoid_att_printer_resolutions_supported,
    pdmoid_att_xp_embedded_formats_supported,
    pdmoid_att_xp_listfonts_modes_supported,
    pdmoid_att_xp_page_attributes_supported,
    pdmoid_att_xp_raw_formats_supported,
    pdmoid_att_xp_setup_proviso,
    pdmoid_att_document_attributes_supported,
    pdmoid_att_job_attributes_supported,
    pdmoid_att_locale,
    pdmoid_att_multiple_documents_supported,
    pdmoid_val_content_orientation_portrait,
    pdmoid_val_content_orientation_landscape,
    pdmoid_val_content_orientation_reverse_portrait,
    pdmoid_val_content_orientation_reverse_landscape,
    pdmoid_val_medium_size_iso_a0,
    pdmoid_val_medium_size_iso_a1,
    pdmoid_val_medium_size_iso_a2,
    pdmoid_val_medium_size_iso_a3,
    pdmoid_val_medium_size_iso_a4,
    pdmoid_val_medium_size_iso_a5,
    pdmoid_val_medium_size_iso_a6,
    pdmoid_val_medium_size_iso_a7,
    pdmoid_val_medium_size_iso_a8,
    pdmoid_val_medium_size_iso_a9,
    pdmoid_val_medium_size_iso_a10,
    pdmoid_val_medium_size_iso_b0,
    pdmoid_val_medium_size_iso_b1,
    pdmoid_val_medium_size_iso_b2,
    pdmoid_val_medium_size_iso_b3,
    pdmoid_val_medium_size_iso_b4,
    pdmoid_val_medium_size_iso_b5,
    pdmoid_val_medium_size_iso_b6,
    pdmoid_val_medium_size_iso_b7,
    pdmoid_val_medium_size_iso_b8,
    pdmoid_val_medium_size_iso_b9,
    pdmoid_val_medium_size_iso_b10,
    pdmoid_val_medium_size_na_letter,
    pdmoid_val_medium_size_na_legal,
    pdmoid_val_medium_size_executive,
    pdmoid_val_medium_size_folio,
    pdmoid_val_medium_size_invoice,
    pdmoid_val_medium_size_ledger,
    pdmoid_val_medium_size_quarto,
    pdmoid_val_medium_size_iso_c3,
    pdmoid_val_medium_size_iso_c4,
    pdmoid_val_medium_size_iso_c5,
    pdmoid_val_medium_size_iso_c6,
    pdmoid_val_medium_size_iso_designated_long,
    pdmoid_val_medium_size_na_10x13_envelope,
    pdmoid_val_medium_size_na_9x12_envelope,
    pdmoid_val_medium_size_na_number_10_envelope,
    pdmoid_val_medium_size_na_7x9_envelope,
    pdmoid_val_medium_size_na_9x11_envelope,
    pdmoid_val_medium_size_na_10x14_envelope,
    pdmoid_val_medium_size_na_number_9_envelope,
    pdmoid_val_medium_size_na_6x9_envelope,
    pdmoid_val_medium_size_na_10x15_envelope,
    pdmoid_val_medium_size_monarch_envelope,
    pdmoid_val_medium_size_a,
    pdmoid_val_medium_size_b,
    pdmoid_val_medium_size_c,
    pdmoid_val_medium_size_d,
    pdmoid_val_medium_size_e,
    pdmoid_val_medium_size_jis_b0,
    pdmoid_val_medium_size_jis_b1,
    pdmoid_val_medium_size_jis_b2,
    pdmoid_val_medium_size_jis_b3,
    pdmoid_val_medium_size_jis_b4,
    pdmoid_val_medium_size_jis_b5,
    pdmoid_val_medium_size_jis_b6,
    pdmoid_val_medium_size_jis_b7,
    pdmoid_val_medium_size_jis_b8,
    pdmoid_val_medium_size_jis_b9,
    pdmoid_val_medium_size_jis_b10,
    pdmoid_val_plex_simplex,
    pdmoid_val_plex_duplex,
    pdmoid_val_plex_tumble,
    pdmoid_val_input_tray_top,
    pdmoid_val_input_tray_middle,
    pdmoid_val_input_tray_bottom,
    pdmoid_val_input_tray_envelope,
    pdmoid_val_input_tray_manual,
    pdmoid_val_input_tray_large_capacity,
    pdmoid_val_input_tray_main,
    pdmoid_val_input_tray_side,
    pdmoid_val_event_report_job_completed,
    pdmoid_val_delivery_method_electronic_mail,
    pdmoid_val_xp_setup_mandatory,
    pdmoid_val_xp_setup_optional,
    pdmoid_val_xp_setup_ok,
    pdmoid_val_xp_setup_incomplete,
    pdmoid_val_xp_list_glyph_fonts,
    pdmoid_val_xp_list_internal_printer_fonts
} PdmOid;
